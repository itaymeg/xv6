#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "fs.h"
int print_var2 = 0;
//#define PRINT {print_var2++; cprintf("#### %d\n", print_var2);}
#define PRINT ;
//define w(x)	{if (proc != 0 && proc->pages.disk.count > 50) cprintf("name: %s\n", #x);}
#define w(x)	;

//#define VALD(x) cprintf("%s = %d\n" ,#x ,x);
#define VALD(x)	;
//#define VALS(x) cprintf("%s = %s\n",#x ,x);
#define VALS(x)	;


pte_t *
walkpgdir(pde_t *pgdir, const void *va, int alloc);

struct {
	struct spinlock lock;
	struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void)
{
	initlock(&ptable.lock, "ptable");
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
	w(allocproc)
																									struct proc *p;
	char *sp;

	acquire(&ptable.lock);
	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
		if(p->state == UNUSED)
			goto found;
	release(&ptable.lock);
	return 0;

	found:
	p->state = EMBRYO;
	p->pid = nextpid++;
	int i;
	for (i = 0; i < MAX_DISC_PAGES; i++){
		p->pages.disk.pageTables[i].virtualAddress = 0;
		p->pages.disk.pageTables[i].used = 0;
		p->pages.disk.pageTables[i].age = 0;
		p->pages.disk.pageTables[i].enterTime = 0;
		if (i < MAX_PSYC_PAGES){
			p->pages.memory.pageTables[i].virtualAddress = 0;
			p->pages.memory.pageTables[i].used = 0;
			p->pages.memory.pageTables[i].age = 0;
			p->pages.memory.pageTables[i].enterTime = 0;
		}
	}
	p->pages.debug = 0;
	p->pages.disk.count = 0;
	p->pages.memory.count = 0;
	p->pages.pageFaults = 0;
	p->pages.totalPagedOut = 0;
	release(&ptable.lock);
	// Allocate kernel stack.
	if((p->kstack = kalloc()) == 0){
		p->state = UNUSED;
		return 0;
	}
	sp = p->kstack + KSTACKSIZE;

	// Leave room for trap frame.
	sp -= sizeof *p->tf;
	p->tf = (struct trapframe*)sp;

	// Set up new context to start executing at forkret,
	// which returns to trapret.
	sp -= 4;
	*(uint*)sp = (uint)trapret;

	sp -= sizeof *p->context;
	p->context = (struct context*)sp;
	memset(p->context, 0, sizeof *p->context);
	p->context->eip = (uint)forkret;
	return p;
}


//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
	w(userinit)
																									calcKernelPages();
	struct proc *p;
	extern char _binary_initcode_start[], _binary_initcode_size[];

	p = allocproc();
	initproc = p;
	if((p->pgdir = setupkvm()) == 0)
		panic("userinit: out of memory?");
	inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
	p->sz = PGSIZE;
	memset(p->tf, 0, sizeof(*p->tf));
	p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
	p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
	p->tf->es = p->tf->ds;
	p->tf->ss = p->tf->ds;
	p->tf->eflags = FL_IF;
	p->tf->esp = PGSIZE;
	p->tf->eip = 0;  // beginning of initcode.S
	safestrcpy(p->name, "initcode", sizeof(p->name));
	p->cwd = namei("/");
	p->state = RUNNABLE;
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
	uint sz;
	w(growproc)
	sz = proc->sz;
	if(n > 0){
		w(proc1)
																										if((sz = allocuvm(proc->pgdir, sz, sz + n)) == 0)
																										{
																											VALD(sz)
																											return -1;
																										}
	} else if(n < 0){
		if((sz = deallocuvm(proc->pgdir, sz, sz + n)) == 0)
			return -1;
	}
	proc->sz = sz;
	switchuvm(proc);
	return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
struct file {
	enum { FD_NONE, FD_PIPE, FD_INODE } type;
	int ref; // reference count
	char readable;
	char writable;
	struct pipe *pipe;
	struct inode *ip;
	uint off;
};

int
fork(void)
{
	w(fork)
																									int i, pid;
	struct proc *np;
	// Allocate process.
	if((np = allocproc()) == 0)
		return -1;

	// Copy process state from p.
	if((np->pgdir = copyuvm(proc->pgdir, proc->sz)) == 0){
		kfree(np->kstack);
		np->kstack = 0;
		np->state = UNUSED;
		return -1;
	}
	np->sz = proc->sz;
	np->parent = proc;
	*np->tf = *proc->tf;
	createSwapFile(np);
	//cprintf("proc count!!!!!!!!!!!!!!!     %d\n", proc->pages.memory.count);
	if (SELECTION != NONE){
		if (proc != initproc && proc != 0 && strncmp("sh",proc->name,3) && proc->swapFile != 0) {
			char buf[PGSIZE/2];
			int toRead;
			for(i=0;i<30;i++){
				toRead = readFromSwapFile(proc,buf,i*MAX_READ_SLICE,MAX_READ_SLICE);
				writeToSwapFile(np,buf,i*MAX_READ_SLICE,toRead);
			}
			//cprintf("proc pid = %d, proc name = %s proc swap = %p\n", proc->pid, proc->name, proc->swapFile);
			//			np->pages = proc->pages;
			int k;
			for (k = 0; k < MAX_DISC_PAGES; k++){
				np->pages.disk.pageTables[k].virtualAddress = proc->pages.disk.pageTables[k].virtualAddress;
				np->pages.disk.pageTables[k].used = proc->pages.disk.pageTables[k].used;
				np->pages.disk.pageTables[k].age = proc->pages.disk.pageTables[k].age;
				np->pages.disk.pageTables[k].enterTime = proc->pages.disk.pageTables[k].enterTime;
				if (k < MAX_PSYC_PAGES){
					np->pages.memory.pageTables[k].virtualAddress = proc->pages.memory.pageTables[k].virtualAddress;
					np->pages.memory.pageTables[k].used = proc->pages.memory.pageTables[k].used;
					np->pages.memory.pageTables[k].age = proc->pages.memory.pageTables[k].age;
					np->pages.memory.pageTables[k].enterTime = proc->pages.memory.pageTables[k].enterTime;
				}
			}

			np->pages.debug = 0;
			np->pages.disk.count = proc->pages.disk.count;
			np->pages.memory.count = proc->pages.memory.count;
			np->pages.pageFaults = proc->pages.pageFaults;
			np->pages.totalPagedOut = proc->pages.totalPagedOut;
			np->swapFile->ip = proc->swapFile->ip;
			np->swapFile->type = proc->swapFile->type;
			np->swapFile->off = proc->swapFile->off;
			np->swapFile->readable = proc->swapFile->readable;
			np->swapFile->writable = proc->swapFile->writable;
		}
	}

	PRINT
	// Clear %eax so that fork returns 0 in the child.
	np->tf->eax = 0;

	for(i = 0; i < NOFILE; i++)
		if(proc->ofile[i])
			np->ofile[i] = filedup(proc->ofile[i]);
	np->cwd = idup(proc->cwd);
	//cprintf("parent name = %s\n", proc->name);
	safestrcpy(np->name, proc->name, sizeof(proc->name));

	pid = np->pid;

	// lock to force the compiler to emit the np->state write last.
	acquire(&ptable.lock);
	np->state = RUNNABLE;
	release(&ptable.lock);

	return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
	struct proc *p;
	int fd;

	if(proc == initproc)
		panic("init exiting");

	// Close all open files.
	for(fd = 0; fd < NOFILE; fd++){
		if(proc->ofile[fd]){
			fileclose(proc->ofile[fd]);
			proc->ofile[fd] = 0;
		}
	}

	begin_op();
	iput(proc->cwd);
	end_op();
	proc->cwd = 0;

	acquire(&ptable.lock);

	// Parent might be sleeping in wait().
	wakeup1(proc->parent);

	// Pass abandoned children to init.
	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if(p->parent == proc){
			p->parent = initproc;
			if(p->state == ZOMBIE)
				wakeup1(initproc);
		}
	}

	// Jump into the scheduler, never to return.
	proc->state = ZOMBIE;
	if (SELECTION != NONE){
		if (proc != 0 && !strncmp("sh",proc->name,2)){
			//removeSwapFile(proc);
			int i;
			for (i = 0; i < MAX_DISC_PAGES; i++){
				proc->pages.disk.pageTables[i].virtualAddress = 0;
				proc->pages.disk.pageTables[i].used = 0;
				proc->pages.disk.pageTables[i].age = 0;
				proc->pages.disk.pageTables[i].enterTime = 0;
				if (i < MAX_PSYC_PAGES){
					proc->pages.memory.pageTables[i].virtualAddress = 0;
					proc->pages.memory.pageTables[i].used = 0;
					proc->pages.memory.pageTables[i].age = 0;
					proc->pages.memory.pageTables[i].enterTime = 0;
				}
			}
			proc->pages.disk.count = 0;
			proc->pages.memory.count = 0;
			proc->pages.pageFaults = 0;
			proc->pages.totalPagedOut = 0;
			if(VERBOSE_PRINT == TRUE) {
				static char *states[] = {
						[UNUSED]    "unused",
						[EMBRYO]    "embryo",
						[SLEEPING]  "sleep ",
						[RUNNABLE]  "runble",
						[RUNNING]   "run   ",
						[ZOMBIE]    "zombie"
				};
				cprintf("%d %s %d %d %d %d %s", p->pid, states[p->state], p->pages.memory.count, p->pages.disk.count, p->pages.pageFaults, p->pages.totalPagedOut,p->name);
			}

		}
	}

	sched();
	panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
	struct proc *p;
	int havekids, pid;

	acquire(&ptable.lock);
	for(;;){
		// Scan through table looking for zombie children.
		havekids = 0;
		for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
			if(p->parent != proc)
				continue;
			havekids = 1;
			if(p->state == ZOMBIE){
				if(VERBOSE_PRINT == TRUE) {
					static char *states[] = {
							[UNUSED]    "unused",
							[EMBRYO]    "embryo",
							[SLEEPING]  "sleep ",
							[RUNNABLE]  "runble",
							[RUNNING]   "run   ",
							[ZOMBIE]    "zombie"
					};
					cprintf("%d %s %d %d %d %d %s", p->pid, states[p->state], p->pages.memory.count, p->pages.disk.count, p->pages.pageFaults, p->pages.totalPagedOut,p->name);
				}
				// Found one.
				pid = p->pid;
				kfree(p->kstack);
				p->kstack = 0;
				freevm(p->pgdir);
				p->state = UNUSED;
				p->pid = 0;
				p->parent = 0;
				p->name[0] = 0;
				p->killed = 0;
				if (SELECTION != NONE){
					pages_info newPages;
					p->pages = newPages;
					//cprintf("##############   %d\n", p->pages.memory.count);
				}
				release(&ptable.lock);
				if (SELECTION != NONE){
					removeSwapFile(p);
				}
				return pid;
			}
		}

		// No point waiting if we don't have any children.
		if(!havekids || proc->killed){
			release(&ptable.lock);
			return -1;
		}

		// Wait for children to exit.  (See wakeup1 call in proc_exit.)
		sleep(proc, &ptable.lock);  //DOC: wait-sleep
	}
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
//void
//updateAge (struct proc* p){
//	int i;
//	pte_t * page;
//	cprintf("GROW %x\n", GROW);
//	for (i = 0; i < MAX_PSYC_PAGES; i++){
//		if (p->pages.memory.pageTables[i].used == PAGE_USED){
//			page = walkpgdir(p->pgdir,(char*) p->pages.memory.pageTables[i].virtualAddress,0);
//			if (*page & PTE_A){	//check if reference bit is on
//				*page = *page & ~PTE_A;		//turn reference off
//				p->pages.memory.pageTables[i].age = p->pages.memory.pageTables[i].age >> 1;
//				p->pages.memory.pageTables[i].age += GROW;
//			}
//			else{
//				p->pages.memory.pageTables[i].age = p->pages.memory.pageTables[i].age >> 1;
//			}
//		}
//	}
//}

void
scheduler(void)
{
	struct proc *p;

	for(;;){
		// Enable interrupts on this processor.
		sti();

		// Loop over process table looking for process to run.
		acquire(&ptable.lock);
		if (SELECTION != NONE){
			for (p = ptable.proc; p < &ptable.proc[NPROC]; p++){
				int i;
				pte_t * page;
				for (i = 0; i < MAX_PSYC_PAGES; i++){
					if (p->pages.memory.pageTables[i].used == PAGE_USED){
						page = walkpgdir(p->pgdir,(char*) p->pages.memory.pageTables[i].virtualAddress,0);
						if (*page & PTE_A){	//check if reference bit is on
							*page = *page & ~PTE_A;		//turn reference off
							p->pages.memory.pageTables[i].age = p->pages.memory.pageTables[i].age >> 1;
							p->pages.memory.pageTables[i].age += GROW;
						}
						else{
							*page = *page & ~PTE_A;		//turn reference off
							p->pages.memory.pageTables[i].age = p->pages.memory.pageTables[i].age >> 1;
						}
					}
					//					}
				}
			}
		}


		for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
			if(p->state != RUNNABLE)
				continue;

			// Switch to chosen process.  It is the process's job
			// to release ptable.lock and then reacquire it
			// before jumping back to us.
			proc = p;
			switchuvm(p);
			p->state = RUNNING;
			swtch(&cpu->scheduler, proc->context);
			switchkvm();

			// Process is done running for now.
			// It should have changed its p->state before coming back.
			proc = 0;
		}
		release(&ptable.lock);

	}
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state.
void
sched(void)
{
	int intena;

	if(!holding(&ptable.lock))
		panic("sched ptable.lock");
	if(cpu->ncli != 1)
		panic("sched locks");
	if(proc->state == RUNNING)
		panic("sched running");
	if(readeflags()&FL_IF)
		panic("sched interruptible");
	intena = cpu->intena;
	swtch(&proc->context, cpu->scheduler);
	cpu->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
	acquire(&ptable.lock);  //DOC: yieldlock
	proc->state = RUNNABLE;
	sched();
	release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
	static int first = 1;
	// Still holding ptable.lock from scheduler.
	release(&ptable.lock);

	if (first) {
		// Some initialization functions must be run in the context
		// of a regular process (e.g., they call sleep), and thus cannot
		// be run from main().
		first = 0;
		iinit(ROOTDEV);
		initlog(ROOTDEV);
	}

	// Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
	if(proc == 0)
		panic("sleep");

	if(lk == 0)
		panic("sleep without lk");

	// Must acquire ptable.lock in order to
	// change p->state and then call sched.
	// Once we hold ptable.lock, we can be
	// guaranteed that we won't miss any wakeup
	// (wakeup runs with ptable.lock locked),
	// so it's okay to release lk.
	if(lk != &ptable.lock){  //DOC: sleeplock0
		acquire(&ptable.lock);  //DOC: sleeplock1
		release(lk);
	}

	// Go to sleep.
	proc->chan = chan;
	proc->state = SLEEPING;
	sched();

	// Tidy up.
	proc->chan = 0;

	// Reacquire original lock.
	if(lk != &ptable.lock){  //DOC: sleeplock2
		release(&ptable.lock);
		acquire(lk);
	}
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
	struct proc *p;

	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
		if(p->state == SLEEPING && p->chan == chan)
			p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
	acquire(&ptable.lock);
	wakeup1(chan);
	release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
	w(kill)
																									struct proc *p;

	acquire(&ptable.lock);
	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if(p->pid == pid){
			p->killed = 1;
			// Wake process from sleep if necessary.
			if(p->state == SLEEPING)
				p->state = RUNNABLE;
			release(&ptable.lock);
			return 0;
		}
	}
	release(&ptable.lock);
	return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
	static char *states[] = {
			[UNUSED]    "unused",
			[EMBRYO]    "embryo",
			[SLEEPING]  "sleep ",
			[RUNNABLE]  "runble",
			[RUNNING]   "run   ",
			[ZOMBIE]    "zombie"
	};
	int i;
	struct proc *p;
	char *state;
	uint pc[10];

	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if(p->state == UNUSED)
			continue;
		if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
			state = states[p->state];
		else
			state = "???";
		if(SELECTION == NONE) {
			cprintf("%d %s %s" ,p->pid, state, p->name);
		} else {
			cprintf("%d %s %d %d %d %d %s", p->pid, state, p->pages.memory.count, p->pages.disk.count, p->pages.pageFaults, p->pages.totalPagedOut,p->name);
		}
		if(p->state == SLEEPING){
			getcallerpcs((uint*)p->context->ebp+2, pc);
			for(i=0; i<10 && pc[i] != 0; i++)
				cprintf(" %p", pc[i]);
		}
		cprintf("\n");
	}
	cprintf("current = %d\n", getCurrentPages());
	cprintf("kernel = %d\n", getKernelPages());
	uint freePages = (getCurrentPages()*100)/getKernelPages();
	cprintf("%d\% free pages in the system\n" ,freePages);
}

//int
//retrivePageFromDisk
