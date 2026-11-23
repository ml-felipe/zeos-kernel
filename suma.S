#include <asm.h>

ENTRY(addASM)

    /** Copied code from the add function translation **/
    push    %ebp
    mov     %esp, %ebp
    push    %eax
    push    %edx
    mov     0x8(%ebp), %edx
    mov     0xC(%ebp), %eax
    add     %edx, %eax
    pop     %eax
    pop     %edx
    mov     %ebp, %esp
    pop     %ebp
    ret