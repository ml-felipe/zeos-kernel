/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

// WRITE VARIABLES
#define LECTURA 0
#define ESCRIPTURA 1
#define BUFFER_SIZE 256
char sys_buffer[BUFFER_SIZE];

// GETTIME VARIABLE
extern int zeos_ticks;

// FORK VARIABLES
extern struct list_head freequeue, readyqueue;
#define PAG_LOG_INIT_FREE PAG_LOG_INIT_CODE + NUM_PAG_CODE // BETWEN CODE AND DATA ZONE
unsigned long nextPID = 1810;

// BLOCK UNBLOCK VARIABLES
extern struct list_head blocked;
extern struct task_struct *idle_task;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int ret_from_fork() {

  return 0;
}

int sys_fork()
{
  // Memory check
  if(list_empty(&freequeue)) return -10; /*ECHILD*/

  // Get task from freequeue
	struct list_head *free = list_first(&freequeue);
	list_del(free);
	union task_union *child_union = (union task_union*) list_head_to_task_struct(free);

  // Copy data
  copy_data((union task_union*) current(), child_union, sizeof(union task_union));

  // Allocate page directory
  allocate_DIR(&child_union->task);

  // Search frames (stack?)
  int child_frames[NUM_PAG_DATA];
  for(int i = 0; i < NUM_PAG_DATA; ++i) {
    child_frames[i] = alloc_frame();
    if(child_frames[i] < 0) {
      // Free frames
      for(int j = 0; j < i; ++j) free_frame(child_frames[j]);
      // Free task
      list_add_tail(&child_union->task.list, &freequeue);
      return -12; /*ENOMEM*/
    }
  }
  
  // Initialize child's address space
  page_table_entry *child_PT = get_PT(&child_union->task);
  page_table_entry *parent_PT = get_PT(current());
  // Share system code and data
  for(int i = 0; i < NUM_PAG_KERNEL; ++i) set_ss_pag(child_PT, i, get_frame(parent_PT, i));
  for(int i = 0; i < NUM_PAG_CODE; ++i) set_ss_pag(child_PT, i + PAG_LOG_INIT_CODE, get_frame(parent_PT, i + PAG_LOG_INIT_CODE));
  // Assign new frames
  for(int i = 0; i < NUM_PAG_DATA; ++i) set_ss_pag(child_PT, i + PAG_LOG_INIT_DATA, child_frames[i]);

  // Inherit user data
  for(int i = 0; i < NUM_PAG_DATA; ++i) {
    // Select parent's free page, copy and delete
    set_ss_pag(parent_PT, i + PAG_LOG_INIT_FREE, child_frames[i]);
    copy_data((void*) ((i + PAG_LOG_INIT_DATA) << 12), (void*) ((i + PAG_LOG_INIT_FREE) << 12), PAGE_SIZE);
    del_ss_pag(parent_PT, i + PAG_LOG_INIT_FREE);
  }
  // Flush */
  set_cr3(current()->dir_pages_baseAddr);

  // Assign pid
  child_union->task.PID = nextPID;

  // Initialize needed values (Needed with copy data already done?)
  child_union->task.quantum = current()->quantum;
  child_union->task.pending_unblocks = current()->pending_unblocks;
  child_union->task.parent = current();
  INIT_LIST_HEAD(&child_union->task.child_list);
	list_add_tail(&child_union->task.child_anchor, &current()->child_list);

  // Prepare for task_switch (Stack offset 5 + 11 + 1)
  child_union->stack[KERNEL_STACK_SIZE - 18] = (unsigned long) &ret_from_fork;
  child_union->stack[KERNEL_STACK_SIZE - 19] = 0;
  child_union->task.kernel_esp = &child_union->stack[KERNEL_STACK_SIZE - 19];

  // Insert in readyqueue
  list_add_tail(&child_union->task.list, &readyqueue);

  // Return PID (and increment for next process)
  return nextPID++;
}

void sys_exit()
{  

  // Free physical memory
  page_table_entry *exit_PT = get_PT(current());
  for(int i = 0; i < NUM_PAG_DATA; ++i) {
    free_frame(get_frame(exit_PT, i + PAG_LOG_INIT_DATA));
    del_ss_pag(exit_PT, i + PAG_LOG_INIT_DATA);
  }

  // Free process and schedule next
  list_del(&current()->child_anchor);
  while(!list_empty(&current()->child_list)){
    struct list_head *child = list_first(&current()->child_list);
		list_del(child);
    list_add_tail(child, &idle_task->child_list);
    list_entry(child, struct task_struct, child_anchor)->parent = idle_task;
  }
  update_process_state_rr(current(), &freequeue);
  sched_next_rr();
  return;
}

int sys_write(int fd, char *buffer, int size) {

  int error = check_fd(fd, ESCRIPTURA);
  if(error != 0) return error;
  else if(buffer == NULL) return -14; /*EFAULT*/
  else if(size < 0) return -22; /*EINVAL*/

  int written = 0;
  while((size - written) > BUFFER_SIZE) { // For bigger buffers
    copy_from_user(buffer + written, sys_buffer, BUFFER_SIZE);
    written += sys_write_console(sys_buffer, BUFFER_SIZE);
  }

  copy_from_user(buffer + written, sys_buffer, size - written); // Rest of the buffer (or whole if smaller)
  written += sys_write_console(sys_buffer, size - written);

  return written;
}

void sys_block(){

  struct task_struct *block = current();
  if(block->pending_unblocks > 0) --block->pending_unblocks;
  else {
    block->pending_unblocks = -1; // To detect blocked processes (should create variable?)
    list_add_tail(&block->list, &blocked); // Blocked queue
    sched_next_rr();  // Scheduler call
  }
}

int sys_unblock(int pid){

  struct list_head *head = &current()->child_list;
  struct list_head *child;
  list_for_each(child, head) {
    struct task_struct *child_task = list_entry(child, struct task_struct, child_anchor);
    if(child_task->PID == pid){
      if(child_task->pending_unblocks == -1) {
        child_task->pending_unblocks = 0;
        list_del(&child_task->list);
        list_add_tail(&child_task->list, &readyqueue);
        return 0;
      }
      else {
        child_task->pending_unblocks++;
        return 0;
      }
    }

  }
  return -1;
}

int sys_gettime(){
  return zeos_ticks;
}