#ifdef VMM_GUEST
#include "hypercall.h"
#endif

        # qemu -kernel loads the kernel at 0x80000000
        # and causes each hart (i.e. CPU) to jump there.
        # kernel.ld causes the following code to
        # be placed at 0x80000000.
.section .text
.global _entry
_entry:
        # set up a stack for C.
        # stack0 is declared in start.c,
        # with a 4096-byte stack per CPU.
        # sp = stack0 + (hartid * 4096)
        la sp, stack0
        li a0, 1024*4
#ifdef VMM_GUEST
        mv a2, a0
        li a7, HC_mhartid
        ecall
        mv a1, a0
        mv a0, a2
#else
        csrr a1, mhartid
#endif
        addi a1, a1, 1
        mul a0, a0, a1
        add sp, sp, a0
#ifndef VMM_GUEST
        # jump to start() in start.c
        call start
#else
        call main
#endif
spin:
        j spin
