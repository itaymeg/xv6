#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;
extern void swap(pde_t *pgdir);
extern int time;


static pte_t *
walkpgdir(pde_t *pgdir, const void *va, int alloc)
{
  pde_t *pde;
  pte_t *pgtab;

  pde = &pgdir[PDX(va)];
  if(*pde & PTE_P){
    pgtab = (pte_t*)p2v(PTE_ADDR(*pde));
  } else {
    if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
      return 0;
    // Make sure all those PTE_P bits are zero.
    memset(pgtab, 0, PGSIZE);
    // The permissions here are overly generous, but they can
    // be further restricted by the permissions in the page table 
    // entries, if necessary.
    *pde = v2p(pgtab) | PTE_P | PTE_W | PTE_U;
  }
  return &pgtab[PTX(va)];
}


static int
mappages(pde_t *pgdir, void *va, uint size, uint pa, int perm)
{
  char *a, *last;
  pte_t *pte;
  
  a = (char*)PGROUNDDOWN((uint)va);
  last = (char*)PGROUNDDOWN(((uint)va) + size - 1);
  for(;;){
    if((pte = walkpgdir(pgdir, a, 1)) == 0)
      return -1;
    if(*pte & PTE_P)
      panic("remap");
    *pte = pa | perm | PTE_P;
    if(a == last)
      break;
    a += PGSIZE;
    pa += PGSIZE;
  }
  return 0;
}

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
  
  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
   char* pageToSwap,*mem;
   int i=0,countPagesInRAM=0;
   int j;
   pte_t* pte;
  if(tf->trapno == T_SYSCALL){
    if(proc->killed)
      exit();
    proc->tf = tf;
    syscall();
    if(proc->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpu->id == 0){
      acquire(&tickslock);
      ticks++;
      if(proc && (SELECTION == NFU)){
      for(j=0;j<15;j++){
	if(proc->existInRAM[j]!=0)
	{
	  pte = walkpgdir(proc->pgdir,proc->pagesInRAM[j].va,0);
	  proc->pagesInRAM[j].aging = (proc->pagesInRAM[j].aging)>>1;
	  if(*pte&PTE_A){
	    proc->pagesInRAM[j].aging = proc->pagesInRAM[j].aging +128;
	  }
	  *pte = (*pte)&(~PTE_A);
	}
      }
      }
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpu->id, tf->cs, tf->eip);
    lapiceoi();
    break;
  case T_PGFLT:

      proc->numOfPageFaults++;
      pageToSwap = (char*)PGROUNDDOWN(rcr2());
      pte_t *pte = walkpgdir(proc->pgdir,pageToSwap,0);
      
      if(!(*pte & PTE_P)){
      // cprintf("------------ trap: T_PGFLT not present ------------\n");
	if(*pte & PTE_PG){
	  //cprintf("------------ trap: T_PGFLT not present and pagedout----------------\n");
	  //cprintf("--------- trap: page found in rcr2 %x -------------\n",pageToSwap);
	  for(i=0;i<15;i++)
	    if(proc->pagesInFile[i]==pageToSwap)
	      break;
	    for(j=0;j<15;j++){
	      if(proc->existInRAM[j]==1)
		countPagesInRAM++;
	    }
	  
	  if((countPagesInRAM == 15) && (!(strncmp("sh",proc->name,3) == 0)) && (!(strncmp("init",proc->name,5) == 0)) && (SELECTION!=NONE)){
	    swap(proc->pgdir);
	  }
	  mem = kalloc();
	  if(mem == 0){
	    cprintf("allocuvm out of memory\n");
	    return;
	  }
	  proc->existInOffset[i]=0;
	
	  memset(mem, 0, PGSIZE);
	  mappages(proc->pgdir, (char*)pageToSwap, PGSIZE, v2p(mem), PTE_W|PTE_U);
	  pte_t *pte = walkpgdir(proc->pgdir, pageToSwap, 0);
	  readFromSwapFile(proc, (char*)p2v(PTE_ADDR(*(pte))), i*PGSIZE, PGSIZE);

	  for(i=0;i<15;i++){
	    if(proc->existInRAM[i]==0)
	      break;
	  }
	  proc->existInRAM[i]=1;
	  proc->pagesInRAM[i].va = (char*)pageToSwap;
	  time++;
	  proc->pagesInRAM[i].ctime = time;
	}
      }

    break;
    
  //PAGEBREAK: 13
  default:
    if(proc == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpu->id, tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            proc->pid, proc->name, tf->trapno, tf->err, cpu->id, tf->eip, 
            rcr2());
    proc->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running 
  // until it gets to the regular system call return.)
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(proc && proc->state == RUNNING && tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit();
}
