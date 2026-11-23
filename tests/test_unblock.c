#include <sched.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
struct list_head readyqueue;
static struct task_struct parent, first, second;
struct task_struct *current(void) { return &parent; }
int sys_unblock(int pid);
static void setup(void) {
 memset(&parent,0,sizeof parent);memset(&first,0,sizeof first);memset(&second,0,sizeof second);
 INIT_LIST_HEAD(&parent.child_list);INIT_LIST_HEAD(&readyqueue);
 first.PID=10;second.PID=20;
}
int main(void) {
 setup();assert(sys_unblock(10)==-1); // Empty child list must not dereference its sentinel.
 list_add_tail(&first.child_anchor,&parent.child_list);
 list_add_tail(&second.child_anchor,&parent.child_list);
 assert(sys_unblock(99)==-1); // Exhaust a nonempty list.
 assert(sys_unblock(20)==0);assert(second.pending_unblocks==1);assert(first.pending_unblocks==0);
 assert(sys_unblock(20)==0);assert(second.pending_unblocks==2);
 struct list_head blocked;INIT_LIST_HEAD(&blocked);
 first.pending_unblocks=-1;list_add_tail(&first.list,&blocked);
 assert(sys_unblock(10)==0);assert(first.pending_unblocks==0);
 assert(list_empty(&blocked));assert(readyqueue.next==&first.list);assert(readyqueue.prev==&first.list);
 assert(sys_unblock(10)==0);assert(first.pending_unblocks==1);assert(readyqueue.next->next==&readyqueue);
 puts("PASS unblock: empty, absent PID, later child, pending notifications, blocked-to-ready transition");
}
