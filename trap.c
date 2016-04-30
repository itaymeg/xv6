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
uint moveToDisk(pde_t *pgdir);
pte_t *
walkpgdir(pde_t *pgdir, const void *va, int alloc);
int
mappages(pde_t *pgdir, void *va, uint size, uint pa, int perm);

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
   
  //PAGEBREAK: 13
  case T_PGFLT:
	  cprintf("page fault! \n");
	  int swapped;
	  int diskPageIdx;
	  int found = 0;
	  int emptySlotMemory;
	  char * retrievedPageMem;
	  uint pageToRetrieve;
	  pageToRetrieve = rcr2();
	  pte_t* pageToRetrieve_pte = walkpgdir(proc->pgdir, (const void*) pageToRetrieve, 0);
	  swapped = !(*pageToRetrieve_pte & PTE_P) & !(*pageToRetrieve_pte & ~PTE_PG);  //check if page was swapped out
	  if (swapped){
		  for (diskPageIdx = 0; diskPageIdx < MAX_PSYC_PAGES; diskPageIdx++){
			  if (proc->pages.disk.pageTables[diskPageIdx].virtualAddress == PGROUNDDOWN(pageToRetrieve)){
				  found = 1;
				  break;
			  }
		  }
//TODO remove debug lines
		  if (!found) cprintf("Page is not in disk!!!!!\n");
//finish debug
		  if (proc->pages.memory.count == MAX_PSYC_PAGES){
			  emptySlotMemory = moveToDisk(proc->pgdir);
		  }
		  else {
			  for (emptySlotMemory = 0; emptySlotMemory < MAX_PSYC_PAGES; emptySlotMemory++){
				  if (proc->pages.memory.pageTables[emptySlotMemory].used == PAGE_UNUSED){
					  proc->pages.memory.pageTables[emptySlotMemory].used = PAGE_USED;
					  break;
				  }
			  }
		  }
		  retrievedPageMem = kalloc();
		  if(retrievedPageMem == 0){
		  	  cprintf("allocuvm out of memory\n");
		  	  return;
		  	}
		  readFromSwapFile(proc,retrievedPageMem,diskPageIdx*PGSIZE,PGSIZE);
		  mappages(proc->pgdir, (char*)PGROUNDDOWN(pageToRetrieve), PGSIZE, v2p(retrievedPageMem), PTE_W|PTE_U);
		  proc->pages.memory.count++;
		  proc->pages.disk.count--;
		  proc->pages.disk.pageTables[diskPageIdx].used = PAGE_UNUSED;
		  proc->pages.memory.pageTables[emptySlotMemory].virtualAddress = PGROUNDDOWN(pageToRetrieve);
	  };
	  break;
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
