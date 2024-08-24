#ifndef __ASSEMBLER__

// which hart (core) is this?
static inline uint64
r_mhartid()
{
  uint64 x;
  asm volatile("csrr %0, mhartid" : "=r" (x) );
  return x;
}

// Machine Status Register, mstatus

#define MSTATUS_MPP_MASK (3L << 11) // previous mode.
#define MSTATUS_MPP_M (3L << 11)
#define MSTATUS_MPP_S (1L << 11)
#define MSTATUS_MPP_U (0L << 11)
#define MSTATUS_VS  (3L << 9)
#define MSTATUS_MIE (1L << 3)    // machine-mode interrupt enable.

static inline uint64
r_mstatus()
{
  uint64 x;
  asm volatile("csrr %0, mstatus" : "=r" (x) );
  return x;
}

static inline void 
w_mstatus(uint64 x)
{
  asm volatile("csrw mstatus, %0" : : "r" (x));
}

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
static inline void 
w_mepc(uint64 x)
{
  asm volatile("csrw mepc, %0" : : "r" (x));
}

// Supervisor Status Register, sstatus

#define SSTATUS_SPP (1L << 8)  // Previous mode, 1=Supervisor, 0=User
#define SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#define SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#define SSTATUS_SIE (1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_UIE (1L << 0)  // User Interrupt Enable

static inline uint64
r_sstatus()
{
  uint64 x;
  asm volatile("csrr %0, sstatus" : "=r" (x) );
  return x;
}

static inline void 
w_sstatus(uint64 x)
{
  asm volatile("csrw sstatus, %0" : : "r" (x));
}

// Supervisor Interrupt Pending
static inline uint64
r_sip()
{
  uint64 x;
  asm volatile("csrr %0, sip" : "=r" (x) );
  return x;
}

static inline void 
w_sip(uint64 x)
{
  asm volatile("csrw sip, %0" : : "r" (x));
}

// Supervisor Interrupt Enable
#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
#define SIE_SSIE (1L << 1) // software
static inline uint64
r_sie()
{
  uint64 x;
  asm volatile("csrr %0, sie" : "=r" (x) );
  return x;
}

static inline void 
w_sie(uint64 x)
{
  asm volatile("csrw sie, %0" : : "r" (x));
}

// Machine-mode Interrupt Enable
#define MIE_MEIE (1L << 11) // external
#define MIE_MTIE (1L << 7)  // timer
#define MIE_MSIE (1L << 3)  // software
static inline uint64
r_mie()
{
  uint64 x;
  asm volatile("csrr %0, mie" : "=r" (x) );
  return x;
}

static inline void 
w_mie(uint64 x)
{
  asm volatile("csrw mie, %0" : : "r" (x));
}

// supervisor exception program counter, holds the
// instruction address to which a return from
// exception will go.
static inline void 
w_sepc(uint64 x)
{
  asm volatile("csrw sepc, %0" : : "r" (x));
}

static inline uint64
r_sepc()
{
  uint64 x;
  asm volatile("csrr %0, sepc" : "=r" (x) );
  return x;
}

// Machine Exception Delegation
static inline uint64
r_medeleg()
{
  uint64 x;
  asm volatile("csrr %0, medeleg" : "=r" (x) );
  return x;
}

static inline void 
w_medeleg(uint64 x)
{
  asm volatile("csrw medeleg, %0" : : "r" (x));
}

// Machine Interrupt Delegation
static inline uint64
r_mideleg()
{
  uint64 x;
  asm volatile("csrr %0, mideleg" : "=r" (x) );
  return x;
}

static inline void 
w_mideleg(uint64 x)
{
  asm volatile("csrw mideleg, %0" : : "r" (x));
}

// Supervisor Trap-Vector Base Address
// low two bits are mode.
static inline void 
w_stvec(uint64 x)
{
  asm volatile("csrw stvec, %0" : : "r" (x));
}

static inline uint64
r_stvec()
{
  uint64 x;
  asm volatile("csrr %0, stvec" : "=r" (x) );
  return x;
}

// Machine-mode interrupt vector
static inline void 
w_mtvec(uint64 x)
{
  asm volatile("csrw mtvec, %0" : : "r" (x));
}

// Physical Memory Protection
static inline void
w_pmpcfg0(uint64 x)
{
  asm volatile("csrw pmpcfg0, %0" : : "r" (x));
}

static inline void
w_pmpaddr0(uint64 x)
{
  asm volatile("csrw pmpaddr0, %0" : : "r" (x));
}

// use riscv's sv39 page table scheme.
#define SATP_SV39 (8L << 60)

#define MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)pagetable) >> 12))

// supervisor address translation and protection;
// holds the address of the page table.
static inline void 
w_satp(uint64 x)
{
  asm volatile("csrw satp, %0" : : "r" (x));
}

static inline uint64
r_satp()
{
  uint64 x;
  asm volatile("csrr %0, satp" : "=r" (x) );
  return x;
}

static inline void 
w_mscratch(uint64 x)
{
  asm volatile("csrw mscratch, %0" : : "r" (x));
}

// Supervisor Trap Cause
static inline uint64
r_scause()
{
  uint64 x;
  asm volatile("csrr %0, scause" : "=r" (x) );
  return x;
}

// Supervisor Trap Value
static inline uint64
r_stval()
{
  uint64 x;
  asm volatile("csrr %0, stval" : "=r" (x) );
  return x;
}

// Machine-mode Counter-Enable
static inline void 
w_mcounteren(uint64 x)
{
  asm volatile("csrw mcounteren, %0" : : "r" (x));
}

static inline uint64
r_mcounteren()
{
  uint64 x;
  asm volatile("csrr %0, mcounteren" : "=r" (x) );
  return x;
}

// machine-mode cycle counter
static inline uint64
r_time()
{
  uint64 x;
  asm volatile("csrr %0, time" : "=r" (x) );
  return x;
}

// enable device interrupts
static inline void
intr_on()
{
  w_sstatus(r_sstatus() | SSTATUS_SIE);
}

// disable device interrupts
static inline void
intr_off()
{
  w_sstatus(r_sstatus() & ~SSTATUS_SIE);
}

// are device interrupts enabled?
static inline int
intr_get()
{
  uint64 x = r_sstatus();
  return (x & SSTATUS_SIE) != 0;
}

static inline uint64
r_sp()
{
  uint64 x;
  asm volatile("mv %0, sp" : "=r" (x) );
  return x;
}

// read and write tp, the thread pointer, which xv6 uses to hold
// this core's hartid (core number), the index into cpus[].
static inline uint64
r_tp()
{
  uint64 x;
  asm volatile("mv %0, tp" : "=r" (x) );
  return x;
}

static inline void 
w_tp(uint64 x)
{
  asm volatile("mv tp, %0" : : "r" (x));
}

static inline uint64
r_ra()
{
  uint64 x;
  asm volatile("mv %0, ra" : "=r" (x) );
  return x;
}

// flush the TLB.
static inline void
sfence_vma()
{
  // the zero, zero means flush all TLB entries.
  asm volatile("sfence.vma zero, zero");
}

typedef uint64 pte_t;
typedef uint64 *pagetable_t; // 512 PTEs

#ifndef VMM_GUEST

#define STR_CSR_HSTATUS "0x600"
#define STR_CSR_HEDELEG "0x602"
#define STR_CSR_HIDELEG "0x603"
#define STR_CSR_HCOUNTEREN "0x606"
#define STR_CSR_HVIP    "0x645"
#define STR_CSR_HGATP   "0x680"

#define STR_CSR_VSSTATUS "0x200"
#define STR_CSR_VSIE     "0x204"
#define STR_CSR_VSTVEC   "0x205"
#define STR_CSR_VSEPC    "0x241"
#define STR_CSR_VSCAUSE  "0x242"
#define STR_CSR_VSTVAL   "0x243"
#define STR_CSR_VSIP     "0x244"

#define HSTATUS_VTSR  (1L << 22)
#define HSTATUS_VTW   (1L << 21)
#define HSTATUS_VTV   (1L << 20)
#define HSTATUS_VGEIN (63L << 12)
#define HSTATUS_HU    (1L << 9)
#define HSTATUS_SPVP  (1L << 8)
#define HSTATUS_SPV   (1L << 7)
#define HSTATUS_GVA   (1L << 6)
#define HSTATUS_VSBE  (1L << 5)

static inline uint64
r_hstatus()
{
  uint64 x;
  asm volatile("csrr %0, " STR_CSR_HSTATUS : "=r" (x) );
  return x;
}

static inline void
w_hstatus(uint64 x)
{
  asm volatile("csrw " STR_CSR_HSTATUS ", %0" :: "r"(x) );
}


#define HGATP_PPN         ((1L << 44) - 1)
#define HGATP_VMID_SHIFT  44
#define HGATP_VMID_SIZE   ( 1L << 14)
#define HGATP_MODE_SV32X4 ( 1L << 60)
#define HGATP_MODE_SV39X4 ( 8L << 60)
#define HGATP_MODE_SV48X4 ( 9L << 60)
#define HGATP_MODE_SV57X4 (10L << 60)

static inline uint64
r_hgatp()
{
  uint64 x;
  asm volatile("csrr %0, " STR_CSR_HGATP : "=r" (x) );
  return x;
}

static inline void
w_hgatp(uint64 x)
{
  asm volatile("csrw " STR_CSR_HGATP ", %0" :: "r"(x) );
}

static inline uint64
r_hedeleg()
{
  uint64 x;
  asm volatile("csrr %0, " STR_CSR_HEDELEG : "=r" (x) );
  return x;
}

static inline void
w_hedeleg(uint64 x)
{
  asm volatile("csrw " STR_CSR_HEDELEG ", %0" :: "r"(x) );
}

static inline uint64
r_hideleg()
{
  uint64 x;
  asm volatile("csrr %0, " STR_CSR_HIDELEG : "=r" (x) );
  return x;
}

static inline void
w_hideleg(uint64 x)
{
  asm volatile("csrw " STR_CSR_HIDELEG ", %0" :: "r"(x) );
}

static inline uint64
r_hcounteren()
{
  uint64 x;
  asm volatile("csrr %0, " STR_CSR_HCOUNTEREN : "=r" (x) );
  return x;
}

static inline void
w_hcounteren(uint64 x)
{
  asm volatile("csrw " STR_CSR_HCOUNTEREN ", %0" :: "r"(x) );
}

static inline uint64
r_hvip()
{
  uint64 x;
  asm volatile("csrr %0, " STR_CSR_HVIP : "=r" (x) );
  return x;
}

static inline void
w_hvip(uint64 x)
{
  asm volatile("csrw " STR_CSR_HVIP ", %0" :: "r"(x) );
}

static inline uint64
r_vsstatus()
{
  uint64 x;
  asm volatile("csrr %0, " STR_CSR_VSSTATUS : "=r" (x) );
  return x;
}

static inline void
w_vsstatus(uint64 x)
{
  asm volatile("csrw " STR_CSR_VSSTATUS ", %0" :: "r"(x) );
}

static inline uint64
r_vsie()
{
  uint64 x;
  asm volatile("csrr %0, " STR_CSR_VSIE : "=r" (x) );
  return x;
}

static inline void
w_vsie(uint64 x)
{
  asm volatile("csrw " STR_CSR_VSIE ", %0" :: "r"(x) );
}

static inline uint64
r_vstvec()
{
  uint64 x;
  asm volatile("csrr %0, " STR_CSR_VSTVEC : "=r" (x) );
  return x;
}

static inline void
w_vstvec(uint64 x)
{
  asm volatile("csrw " STR_CSR_VSTVEC ", %0" :: "r"(x) );
}

static inline uint64
r_vsepc()
{
  uint64 x;
  asm volatile("csrr %0, " STR_CSR_VSEPC : "=r" (x) );
  return x;
}

static inline void
w_vsepc(uint64 x)
{
  asm volatile("csrw " STR_CSR_VSEPC ", %0" :: "r"(x) );
}

static inline uint64
r_vscause()
{
  uint64 x;
  asm volatile("csrr %0, " STR_CSR_VSCAUSE : "=r" (x) );
  return x;
}

static inline void
w_vscause(uint64 x)
{
  asm volatile("csrw " STR_CSR_VSCAUSE ", %0" :: "r"(x) );
}

static inline uint64
r_vstval()
{
  uint64 x;
  asm volatile("csrr %0, " STR_CSR_VSTVAL : "=r" (x) );
  return x;
}

static inline void
w_vstval(uint64 x)
{
  asm volatile("csrw " STR_CSR_VSTVAL ", %0" :: "r"(x) );
}

static inline uint64
r_vsip()
{
  uint64 x;
  asm volatile("csrr %0, " STR_CSR_VSIP : "=r" (x) );
  return x;
}

static inline void
w_vsip(uint64 x)
{
  asm volatile("csrw " STR_CSR_VSIP ", %0" :: "r"(x) );
}

#endif // VMM_GUEST

#endif // __ASSEMBLER__

#define PGSIZE 4096 // bytes per page
#define PGSHIFT 12  // bits of offset within a page

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) // user can access

// shift a physical address to the right place for a PTE.
#define PA2PTE(pa) ((((uint64)pa) >> 12) << 10)

#define PTE2PA(pte) (((pte) >> 10) << 12)

#define PTE_FLAGS(pte) ((pte) & 0x3FF)

// extract the three 9-bit page table indices from a virtual address.
#define PXMASK          0x1FF // 9 bits
#define PXSHIFT(level)  (PGSHIFT+(9*(level)))
#define PX(level, va) ((((uint64) (va)) >> PXSHIFT(level)) & PXMASK)

// one beyond the highest possible virtual address.
// MAXVA is actually one bit less than the max allowed by
// Sv39, to avoid having to sign-extend virtual addresses
// that have the high bit set.
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))

#define CSR_HSTATUS 0x600
#define CSR_HEDELEG 0x602
#define CSR_HIDELEG 0x603
#define CSR_HCOUNTEREN 0x606
#define CSR_HVIP    0x645
#define CSR_HGATP   0x680
