#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "stat.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"
#include "hypercall.h"
#include "elf.h"

int nextvmid = 1;
struct spinlock vmid_lock;

int flags2perm(int);
void runguest(void);
void guesttrap(void);
void hypercall(void);

void
vmminit(void)
{
  initlock(&vmid_lock, "nextvmid");
}

int
allocvmid(void)
{
  int vmid;

  acquire(&vmid_lock);
  vmid = nextvmid;
  nextvmid = nextvmid + 1;
  release(&vmid_lock);

  return vmid;
}

static int
allocguest(struct proc *p, uint64 size)
{
  // call allocvmid and assign that id to proc
  p->vmid = allocvmid();
  
  // assign proc page table
  p->stage_pagetable = proc_pagetable(p);
  return 0;
}

static int
loadseg(pagetable_t pagetable, uint64 va, struct inode *ip, uint offset, uint sz)
{
  uint i, n;
  uint64 pa;

  for(i = 0; i < sz; i += PGSIZE){
    pa = walkaddr(pagetable, va + i);
    if(pa == 0)
      panic("loadseg: address should exist");
    if(sz - i < PGSIZE)
      n = sz - i;
    else
      n = PGSIZE;
    if(readi(ip, 0, (uint64)pa, offset+i, n) != n)
      return -1;
  }

  return 0;
}

static int
loadguest(struct proc *p, char *path)
{
  int i, off;
  uint64 sz = KERNBASE;
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pagetable_t pagetable = p->stage_pagetable;

  begin_op();

  if((ip = namei(path)) == 0){
    end_op();
    return -1;
  }
  ilock(ip);
  if(readi(ip, 0, (uint64)&elf, 0, sizeof(elf)) != sizeof(elf))
    goto bad;

  if(elf.magic != ELF_MAGIC)
    goto bad;

  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, 0, (uint64)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(ph.vaddr + ph.memsz < ph.vaddr)
      goto bad;
    if(ph.vaddr % PGSIZE != 0)
      goto bad;
    uint64 sz1;
    if((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz, flags2perm(ph.flags))) == 0)
      goto bad;
    sz = sz1;
    if(loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      goto bad;
  }
  iunlockput(ip);
  end_op();
  return 0;
 bad:
  if(ip){
    iunlockput(ip);
    end_op();
  }
  return -1;
}

uint64
sys_mkguest(void)
{
  char path[MAXPATH];
  struct proc *np;
  int n, sz;
  int pid;

  if((n = argstr(0, path, MAXPATH)) < 0)
    return 0;

  argint(1, &sz);

  np = allocproc();
  if (!np)
    return 0;

  np->sz = sz;

  
  np->context.ra = (uint64)runguest;  
  pid = np->pid;
  release(&np->lock);

  if (allocguest(np, sz) < 0) {
    freeproc(np);
    return 0;
  }

  if (loadguest(np, path) < 0) {
    freeproc(np);
    return 0;
  }

  // acquire the lock and set the memory to np for gtrapframe
  acquire(&np->lock);
  memset((char*)np->gtrapframe, 0, PGSIZE);

  // Mark the state as runnable
  np->state = RUNNABLE;

  // Mark current myproc as parent
  np->parent = myproc();
  np->gtrapframe->guest_hstatus = HSTATUS_VTW | HSTATUS_SPVP | HSTATUS_SPV;
  np->gtrapframe->guest_sepc = KERNBASE;
  np->gtrapframe->guest_sstatus = SSTATUS_SPP | SSTATUS_SPIE;

  // release the lock once we set the flags
  release(&np->lock);

  return pid;
}

extern void switch_to_guest(struct gtrapframe *);

void
runguest(void)
{
  // Still holding p->lock from scheduler.
  release(&myproc()->lock);

  intr_off();

  while(1) {
    struct proc *p = myproc();
    uint64 hgatp = HGATP_MODE_SV39X4;
    hgatp |= ((uint64) p->vmid) << HGATP_VMID_SHIFT;
    hgatp |= (((uint64) p->stage_pagetable) >> PGSHIFT) & HGATP_PPN;
    w_hgatp(hgatp);
    w_hedeleg((1L << 0) | (1L << 3) | (1L << 8) | (1L << 12) | (1L << 13) | (1L << 15));
    w_hideleg((1L << 2) | (1L << 6) | (1L << 10));
    w_hcounteren(0x2);
    w_hvip(0);

    // Start the code of switching to the guest
    switch_to_guest((struct gtrapframe *) p->trapframe);

    // Returned from the guest, there is something to handle
    guesttrap();
  }
}

extern int devintr(void);

void
delegate_to_guest(void)
{
  struct proc *p = myproc();

  /* Set the guest's SPP, SPIE, and SIE bits */
  uint64 vsstatus = r_vsstatus();

  /* If interrrupts are disabled in the guest, ignore for now */
  if (!(vsstatus & SSTATUS_SIE))
    return;

  vsstatus &= ~SSTATUS_SPP;
  if (p->gtrapframe->guest_sstatus & SSTATUS_SPP)
    vsstatus |= SSTATUS_SPP;
  vsstatus &= ~SSTATUS_SPIE;
  if (p->gtrapframe->guest_sstatus & SSTATUS_SIE)
    vsstatus |= SSTATUS_SPIE;
  vsstatus &= ~SSTATUS_SIE;
  w_vsstatus(vsstatus);

  w_vscause(r_scause());
  w_vstval(r_stval());
  w_vsepc(p->gtrapframe->guest_sepc);

  /* Set the guest PC to the guest exception vector */
  p->gtrapframe->guest_sepc = r_vstvec();

  /* Set the guest to the supervisor mode */
  p->gtrapframe->guest_sstatus |= SSTATUS_SPP;
}

void
guesttrap(void)
{
  int which_dev;
  struct proc *p = myproc();

  // TODO: You may have to handle other traps from the guest kernel

  if(r_scause() == 10){
    // hypercall

    if(killed(p))
      exit(-1);

    p->gtrapframe->guest_sepc += 4;

    intr_on();

    hypercall();

    intr_off();
  } else if((which_dev = devintr()) != 0){
    // ok
    if (which_dev == 2) {
      /* If the guest hasn't set stvec, just ignore the trap */
      if (r_vstvec() == 0)
        w_vsip(r_vsip() & ~2);
      else
        delegate_to_guest();
    }
  } else {
    printf("guesttrap(): unexpected scause %p vmid=%d\n", r_scause(), p->vmid);
    printf("             sepc=%p stval=%p\n", r_sepc(), r_stval());
    setkilled(p);
  }

  if (killed(p))
    exit(-1);
}

// Use this function to retrieve arguments from hypercalls
static
__attribute__((unused))
uint64
argraw(int n)
{
  struct proc *p = myproc();
  switch (n) {
  case 0:
    return p->gtrapframe->guest.a0;
  case 1:
    return p->gtrapframe->guest.a1;
  case 2:
    return p->gtrapframe->guest.a2;
  case 3:
    return p->gtrapframe->guest.a3;
  case 4:
    return p->gtrapframe->guest.a4;
  case 5:
    return p->gtrapframe->guest.a5;
  }
  panic("argraw");
  return -1;
}

uint64 hc_consolewrite(void)
{
  uint64 src = argraw(0);
  int n = argraw(1);
  int sync = argraw(2);
  int i = 0;
  struct proc* p = myproc();

  void* buf = kalloc();
  if (buf == 0)
    panic("kalloc in hc_consolewrite");

  while(i < n) {
    int r = n - i;
    if (r > PGSIZE) r = PGSIZE;
    if (copyin(p->stage_pagetable, buf, src+i, r) == -1)
      break;
    if (sync) {
        for (int j = 0; j < r; j++)
          uartputc_sync(*(char *)(buf + j));
    } else
        consolewrite(0, (uint64)buf, r);
    i += r;
  }

  kfree(buf);
  return i;
}

uint64 hc_consoleread(void)
{
  uint64 dst = argraw(0);
  int n = argraw(1);
  int i = 0;
  struct proc* p = myproc();

  void* buf = kalloc();
  if (buf == 0)
    panic("kalloc in hc_consoleread");

  while(i < n) {
    int r = n - i, s;
    if (r > PGSIZE) r = PGSIZE;
    s = consoleread(0, (uint64)buf, r);
    if (copyout(p->stage_pagetable, dst+i, buf, r) == -1)
      break;
    i += s;
    if (s < r)
      break;
  }

  kfree(buf);
  return i;
}

uint64 hc_memsize(void)
{
  return myproc()->sz;
}

uint64 hc_mhartid(void)
{
  // get core id/num
  return r_tp();
}


// An array mapping hypercall numbers to the function that
// handles the hypercall.
static uint64 (*hypercalls[])(void) = {
[HC_mhartid] hc_mhartid,
[HC_consolewrite] hc_consolewrite,
[HC_consoleread]  hc_consoleread,
[HC_memsize]    hc_memsize,
};

void
hypercall(void)
{
  int num;
  struct proc *p = myproc();

  num = p->gtrapframe->guest.a7;
  if(num > 0 && num < NELEM(hypercalls) && hypercalls[num]) {
    // Use num to lookup the hypercall function for num, call it,
    // and store its return value in p->gtrapframe->guest.a0
    p->gtrapframe->guest.a0 = hypercalls[num]();
  } else {
    printf("guest %d: unknown hypercall %d\n", p->vmid, num);
    p->gtrapframe->guest.a0 = -1;
  }
}
