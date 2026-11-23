#include <asm.h>

// void task_switch(union task_union *new)
ENTRY(task_switch)
	pushl %ebp
	movl %esp, %ebp
	pushl %esi
	pushl %edi
	pushl %ebx
	pushl 8(%ebp)
	call inner_task_switch
	addl $4, %esp
	popl %ebx
	popl %edi
	popl %esi
	popl %ebp
	ret

// void switch_stack(void ** sp_to_save, void * new_sp)
ENTRY(switch_stack)
	mov 4(%esp), %eax
	mov %ebp, (%eax)
	mov 8(%esp), %esp
	popl %ebp
	ret