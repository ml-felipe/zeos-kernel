# ZeOS kernel coursework

Recovered UPC-FIB Operating Systems coursework: a small 32-bit x86 teaching kernel with process scheduling, fork/exit, block/unblock system calls, interrupt handling, memory-management code, user wrappers and a test user program.

The source is the coherent `zeos23-4` Drive variant, including its headers, linker scripts, Makefile and Bochs/GDB configuration. It contains the block/unblock path and separate stack-switch assembly. The older `zeos` archive and the alternative `zeosNEW` statistics/yield work were compared but not merged: their structures and implementation differ. This selection is not a claim that it was the final submitted version.

## Build and test

Run `make` for the original image build. It expects GNU C/binutils with 32-bit x86 support, `as86` and `ld86` from dev86, and the course `libzeos.a` in the project root. That static library is not included: only a prebuilt copy was recoverable, with no corresponding source or redistribution grant established. `make emul` additionally needs Bochs. The original configuration files are retained. Avoid the legacy `make disk` target unless intentionally writing a floppy device.

Run `make -C tests test` for the host regression test (C compiler and GNU-compatible linker). It links the actual `sys_unblock` implementation and list operations, with a test-only `current()` replacement and unused kernel sections discarded. It covers an empty child list, an absent PID, a later child, queued wakeups and a blocked-to-ready transition. This passed on GCC 16.2.1. It does not exercise interrupts, context switches or process execution.

A full build was attempted and stopped at missing `as86`. No image was booted; this repository is **not verified as a bootable OS**. A separate `make -B system user` attempt stopped at the undeclared `schedule()` call in `interrupt.c` under GCC 16.2.1; it also reported multibyte character constants in the keyboard table. These legacy compiler/encoding issues and the unavailable course library remain integration limitations.

## Repairs

`sys_unblock` now iterates child-list entries without treating the sentinel as a task. The initial user task is linked into its parent's child list, matching the unconditional removal in `sys_exit`. The `user.o` recipe now uses the compiler's `-o` option instead of the invalid `-user.o` argument. The kernel design and remaining coursework code are preserved.

See [ATTRIBUTION.md](ATTRIBUTION.md) for provenance and publication restrictions.
