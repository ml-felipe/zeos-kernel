#include <asm.h>

#define RESULT_PROCESS(sys_call_return) \
    cmpl $0, %eax; \
    jge sys_call_return; \
    negl %eax; \
    leal errno, %edx; \
    movl %eax, (%edx); \
    movl $-1, %eax

ENTRY(exit)
    pushl %ebp
    movl %esp, %ebp

    movl $1, %eax         // exit code

    int $0x80

    RESULT_PROCESS(exit_return)
    exit_return:
    movl %ebp, %esp
    popl %ebp

    ret

ENTRY(fork)
    pushl %ebp
    movl %esp, %ebp

    movl $2, %eax         // fork code

    int $0x80

    RESULT_PROCESS(fork_return)
    fork_return:
    movl %ebp, %esp
    popl %ebp

    ret

ENTRY(write)
    pushl %ebp
    movl %esp, %ebp
    pushl %ebx

    movl 8(%ebp), %edx;    // fd
    movl 12(%ebp), %ecx    // *buffer
    movl 16(%ebp), %ebx    // size

    movl $4, %eax         // write code

    int $0x80

    RESULT_PROCESS(write_return)
    write_return:
    popl %ebx
    movl %ebp, %esp
    popl %ebp

    ret

ENTRY(writeMSR)
    pushl %ebp
    movl %esp, %ebp
    pushl %ebx

    movl 8(%ebp), %edx;    // fd
    movl 12(%ebp), %ecx    // *buffer
    movl 16(%ebp), %ebx    // size

    movl $4, %eax         // write code

    pushl %ecx
    pushl %edx
    pushl $writeMSR_sysenter_return
    pushl %ebp
    movl %esp, %ebp
    sysenter

    writeMSR_sysenter_return:
    popl %ebp
    addl $4, %esp
    popl %edx
    popl %ecx

    RESULT_PROCESS(writeMSR_return)
    writeMSR_return:
    popl %ebx
    movl %ebp, %esp
    popl %ebp

    ret

ENTRY(block)
    pushl %ebp
    movl %esp, %ebp

    movl $5, %eax         // block code

    int $0x80

    RESULT_PROCESS(block_return)
    block_return:
    movl %ebp, %esp
    popl %ebp

    ret

ENTRY(unblock)
    pushl %ebp
    movl %esp, %ebp

    movl 8(%ebp), %edx;    // pid

    movl $6, %eax         // unblock code

    int $0x80

    RESULT_PROCESS(unblock_return)
    unblock_return:
    movl %ebp, %esp
    popl %ebp

    ret

ENTRY(gettime)
    pushl %ebp
    movl %esp, %ebp

    movl $10, %eax         // gettime code

    int $0x80

    RESULT_PROCESS(gettime_return)
    gettime_return:
    movl %ebp, %esp
    popl %ebp

    ret

ENTRY(getpid)
    pushl %ebp
    movl %esp, %ebp

    movl $20, %eax         // getpid code

    int $0x80

    RESULT_PROCESS(getpid_return)
    getpid_return:
    movl %ebp, %esp
    popl %ebp

    ret