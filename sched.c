/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));


struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}

extern struct list_head blocked;
struct list_head freequeue, readyqueue;
struct task_struct *idle_task;

int current_quantum;

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void init_idle (void)
{
	// Get task from freequeue
	struct list_head *free = list_first(&freequeue);
	list_del(free);
	union task_union *idle_union = (union task_union*) list_head_to_task_struct(free);

	// Assign process characteristics
	idle_union->task.PID = 0;
	idle_union->task.quantum = 6;
	idle_union->task.pending_unblocks = 0;
	idle_union->task.parent = &idle_union->task; // Initialized but won't be used
	INIT_LIST_HEAD(&idle_union->task.child_list);
	allocate_DIR(&idle_union->task);

	// Context
	idle_union->stack[KERNEL_STACK_SIZE - 1] = (unsigned long) &cpu_idle; // Address
	idle_union->stack[KERNEL_STACK_SIZE - 2] = 0; // Initial value
	idle_union->task.kernel_esp = &idle_union->stack[KERNEL_STACK_SIZE - 2];
	idle_task = &idle_union->task;

}

void init_task1(void)
{
	// Get task from freequeue
	struct list_head *free = list_first(&freequeue);
	list_del(free);
	union task_union *task1_union = (union task_union*) list_head_to_task_struct(free);
	
	// Assign process characteristics
	task1_union->task.PID = 1;
	task1_union->task.quantum = 6;
	task1_union->task.pending_unblocks = 0;
	task1_union->task.parent = idle_task;
	INIT_LIST_HEAD(&task1_union->task.child_list);
	list_add_tail(&task1_union->task.child_anchor, &idle_task->child_list);
	allocate_DIR(&task1_union->task);

	// Adress space
	set_user_pages(&task1_union->task);

	// Update TSS
	tss.esp0 = KERNEL_ESP(task1_union);

	// Update MSR
	writeMSR_handler(0x175, KERNEL_ESP(task1_union));

	// Update system's page directory
	set_cr3(task1_union->task.dir_pages_baseAddr);

}

void init_sched()
{
	INIT_LIST_HEAD(&freequeue);
	INIT_LIST_HEAD(&readyqueue);
	INIT_LIST_HEAD(&blocked);
	for(int i = 0; i < NR_TASKS; ++i) list_add_tail(&task[i].task.list, &freequeue);
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

void update_sched_rr(){

	--current_quantum;
}

int needs_sched_rr(){

	if(current_quantum > 0) return 0; // Still executing
	else if(list_empty(&readyqueue)) {
		current_quantum = current()->quantum; // No other process, reset quantum
		return 0;
	}
	else return 1;
}

void update_process_state_rr(struct task_struct *t, struct list_head *dst_queue) {
	
	struct list_head *aux = &t->list;
	if(aux->prev != NULL && aux->next != NULL) list_del(aux); // Delete from list if existant
	if(dst_queue != NULL) list_add_tail(aux, dst_queue); // Add to list if existant

}

void sched_next_rr(){

	// Assign new process
	union task_union *new;
	if(list_empty(&readyqueue)) new = (union task_union*) idle_task; // No available processes
	else {
		struct list_head *ready = list_first(&readyqueue);
		list_del(ready);
		new = (union task_union*) list_head_to_task_struct(ready);
	}

	current_quantum = new->task.quantum; // Update Quantum
	task_switch(new);

}

void schedule(){

	update_sched_rr();
	if(needs_sched_rr()){ 
		if(current() != idle_task) update_process_state_rr(current(), &readyqueue);
		sched_next_rr();
	}

}

int get_quantum(struct task_struct *t){

	return t->quantum;
}

void set_quantum(struct task_struct *t, int new_quantum) {

	t->quantum = new_quantum;
}

void inner_task_switch(union task_union *new) {

	// Update TSS
	tss.esp0 = KERNEL_ESP(new);

	// Update MSR
	writeMSR_handler(0x175, (long) KERNEL_ESP(new)); /* Both used therefore both needed */

	// Update system's page directory
	set_cr3(new->task.dir_pages_baseAddr);

	switch_stack((void **) &current()->kernel_esp, (void *) new->task.kernel_esp);
}