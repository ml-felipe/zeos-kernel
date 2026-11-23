/*
 * entry.h - Definició del punt d'entrada de les crides al sistema
 */

#ifndef __ENTRY_H__
#define __ENTRY_H__

void re_page_fault_handler();
void keyboard_handler();
void clock_handler();

void writeMSR_handler(long msr, long value);

void system_call_handler();
void syscall_handler_sysenter();

#endif  /* __ENTRY_H__ */
