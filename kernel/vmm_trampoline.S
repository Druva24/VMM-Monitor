        #
        # low-level code to handle traps from user space into
        # the kernel, and returns from kernel to user.
        #
        # the kernel maps the page holding this code
        # at the same virtual address (TRAMPOLINE)
        # in user and kernel space so that it continues
        # to work when it switches page tables.
        # kernel.ld causes this code to start at 
        # a page boundary.
        #

#include "riscv.h"

.globl vmm_trampoline
vmm_trampoline:
.align 4
.globl switch_to_guest
switch_to_guest:
        # switch_to_guest(gtrapframe)
        # called by handle_vmexit() in vmm.c to
        # switch from vmm to guest kernel.
        # a0: trapframe

        # store host GPRs
        sd ra, 0(a0)
        sd sp, 8(a0)
        sd gp, 16(a0)
        sd tp, 24(a0)
        sd s0, 56(a0)
        sd s1, 64(a0)
        sd a1, 80(a0)
        sd a2, 88(a0)
        sd a3, 96(a0)
        sd a4, 104(a0)
        sd a5, 112(a0)
        sd a6, 120(a0)
        sd a7, 128(a0)
        sd s2, 136(a0)
        sd s3, 144(a0)
        sd s4, 152(a0)
        sd s5, 160(a0)
        sd s6, 168(a0)
        sd s7, 176(a0)
        sd s8, 184(a0)
        sd s9, 192(a0)
        sd s10, 200(a0)
        sd s11, 208(a0)

        # load guest CSRs
        ld t0, 536(a0)
        ld t1, 544(a0)
        ld t2, 552(a0)
        la t4, _return_from_guest
        ld t5, 560(a0)

        # save host and restore guest SSTATUS, HSTATUS, SCOUNTEREN, STVEC, and SSCRATCH
        csrrw t0, sstatus, t0
        csrrw t1, CSR_HSTATUS, t1
        csrrw t2, scounteren, t2
        csrrw t4, stvec, t4
        csrrw t3, sscratch, a0

        # restore guest SEPC
        csrw sepc, t5

        # store host CSRs
        sd t0, 496(a0)
        sd t1, 504(a0)
        sd t2, 512(a0)
        sd t3, 520(a0)
        sd t4, 528(a0)

        # restore all guest GPRs but a0
        ld ra, 248(a0)
        ld sp, 256(a0)
        ld gp, 264(a0)
        ld tp, 272(a0)
        ld t0, 280(a0)
        ld t1, 288(a0)
        ld t2, 296(a0)
        ld s0, 304(a0)
        ld s1, 312(a0)
        ld a1, 328(a0)
        ld a2, 336(a0)
        ld a3, 344(a0)
        ld a4, 352(a0)
        ld a5, 360(a0)
        ld a6, 368(a0)
        ld a7, 376(a0)
        ld s2, 384(a0)
        ld s3, 392(a0)
        ld s4, 400(a0)
        ld s5, 408(a0)
        ld s6, 416(a0)
        ld s7, 424(a0)
        ld s8, 432(a0)
        ld s9, 440(a0)
        ld s10, 448(a0)
        ld s11, 456(a0)
        ld t3, 464(a0)
        ld t4, 472(a0)
        ld t5, 480(a0)
        ld t6, 488(a0)

        # restore guest a0
        ld a0, 320(a0)

_before_return_to_guest:
        # return to guest kernel
        sret

.align 4
_return_from_guest:
        # swap guest a0 with sscratch
        csrrw a0, sscratch, a0

        # save all guest GPRs but a0
        sd ra, 248(a0)
        sd sp, 256(a0)
        sd gp, 264(a0)
        sd tp, 272(a0)
        sd t0, 280(a0)
        sd t1, 288(a0)
        sd t2, 296(a0)
        sd s0, 304(a0)
        sd s1, 312(a0)
        sd a1, 328(a0)
        sd a2, 336(a0)
        sd a3, 344(a0)
        sd a4, 352(a0)
        sd a5, 360(a0)
        sd a6, 368(a0)
        sd a7, 376(a0)
        sd s2, 384(a0)
        sd s3, 392(a0)
        sd s4, 400(a0)
        sd s5, 408(a0)
        sd s6, 416(a0)
        sd s7, 424(a0)
        sd s8, 432(a0)
        sd s9, 440(a0)
        sd s10, 448(a0)
        sd s11, 456(a0)
        sd t3, 464(a0)
        sd t4, 472(a0)
        sd t5, 480(a0)
        sd t6, 488(a0)

        # restore all host CSRs
        ld t1, 528(a0)
        ld t2, 520(a0)
        ld t3, 512(a0)
        ld t4, 504(a0)
        ld t5, 496(a0)

        # save guest and restore host SEPC, SSCRATCH, STVEC, SCOUNTEREN, HSTATUS, SSTATUS
        csrr t0, sepc
        csrrw t2, sscratch, t2
        csrw stvec, t1
        csrrw t3, scounteren, t3
        csrrw t4, CSR_HSTATUS, t4
        csrrw t5, sstatus, t5

        # save all guest CSRs
        sd t0, 560(a0)
        sd t2, 320(a0)
        sd t3, 552(a0)
        sd t4, 544(a0)
        sd t5, 536(a0)

        # restore all host GPRs but a0 and t0-t6
        ld ra, 0(a0)
        ld sp, 8(a0)
        ld gp, 16(a0)
        ld tp, 24(a0)
        ld s0, 56(a0)
        ld s1, 64(a0)
        ld a1, 80(a0)
        ld a2, 88(a0)
        ld a3, 96(a0)
        ld a4, 104(a0)
        ld a5, 112(a0)
        ld a6, 120(a0)
        ld a7, 128(a0)
        ld s2, 136(a0)
        ld s3, 144(a0)
        ld s4, 152(a0)
        ld s5, 160(a0)
        ld s6, 168(a0)
        ld s7, 176(a0)
        ld s8, 184(a0)
        ld s9, 192(a0)
        ld s10, 200(a0)
        ld s11, 208(a0)

        # return to C code
        ret
