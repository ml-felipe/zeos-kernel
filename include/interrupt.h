/*
 * interrupt.h - Definició de les diferents rutines de tractament d'exepcions
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <types.h>

#define IDT_ENTRIES 256

extern Gate idt[IDT_ENTRIES];
extern Register idtR;

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL);
void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL);

void setIdt();

// EXCEPTIONS REDEFINITIONS

void re_page_fault_routine(unsigned int eip);

// HANDLERS
extern void re_page_fault_handler();
extern void keyboard_handler();
extern void clock_handler();

extern void writeMSR_handler(long msr, long value);

extern void system_call_handler();
extern void syscall_handler_sysenter();

#endif  /* __INTERRUPT_H__ */
