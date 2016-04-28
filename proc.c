#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

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

int 
allocpid(void) 
{
	int pid;
	do {
		pid = nextpid;
	}while (!cas(&nextpid,nextpid,nextpid+1));
	return pid;
}
//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
	struct proc *p;
	char *sp;

	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
		if(cas(&(p->state), UNUSED, EMBRYO))
			goto found;
	return 0;

	found:

	p->pid = allocpid();
	p->busy = 0;
	p->sighandler = (sig_handler)-1;
	int idx;
	for(idx = 0; idx < 10; idx++){
		p->pending_signals.frame[idx].used = 0;
	}
	p->pending_signals.head = &(p->pending_signals.frame[0]);

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

	sz = proc->sz;
	if(n > 0){
		if((sz = allocuvm(proc->pgdir, sz, sz + n)) == 0)
			return -1;
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
int
fork(void)
{
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
	np->sighandler = proc->sighandler;
	np->busy = 0;
	*np->tf = *proc->tf;

	// Clear %eax so that fork returns 0 in the child.
	np->tf->eax = 0;

	for(i = 0; i < NOFILE; i++)
		if(proc->ofile[i])
			np->ofile[i] = filedup(proc->ofile[i]);
	np->cwd = idup(proc->cwd);

	safestrcpy(np->name, proc->name, sizeof(proc->name));

	pid = np->pid;

	// lock to force the compiler to emit the np->state write last.
	//TODO
	//delete old lock
	//acquire(&ptable.lock);
	pushcli();
	//np->state = RUNNABLE;
	if(!cas(&(np->state), EMBRYO, RUNNABLE)){
		popcli();
		return -1;
	}
	popcli();
	//release(&ptable.lock);

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

	//acquire(&ptable.lock);  
	pushcli();
	if(!cas(&(proc->state), RUNNING, _ZOMBIE)){
		return;
	}

	//proc->state = ZOMBIE;

	// Parent might be sleeping in wait().
	wakeup1(proc->parent);

	// Pass abandoned children to init.
	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if(p->parent == proc){
			p->parent = initproc;
			if(p->state == ZOMBIE || p->state == _ZOMBIE)
				wakeup1(initproc);
		}
	}

	// Jump into the scheduler, never to return.

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

	//acquire(&ptable.lock);
	pushcli();
	for(;;){
		proc->chan = (int)proc;
		//proc->state = SLEEPING;
		if(!cas(&(proc->state),RUNNING, _SLEEPING)){
			popcli();
			return -1;
		}
		// Scan through table looking for zombie children.
		havekids = 0;
		for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
			if(p->parent != proc)
				continue;
			havekids = 1;
			while(p->state == _ZOMBIE);
			if(p->state == ZOMBIE){
				// Found one.
				pid = p->pid;
				//p->state = UNUSED;
				p->pid = 0;
				p->parent = 0;
				p->name[0] = 0;
				proc->chan = 0;
				cas(&(p->state),ZOMBIE, UNUSED);
				if (!cas(&(proc->state),_SLEEPING, RUNNING)){
					cas(&(proc->state),_RUNNABLE, RUNNING);
				}
				//release(&ptable.lock);
				popcli();
				return pid;
			}
		}

		// No point waiting if we don't have any children.
		if(!havekids || proc->killed){
			proc->chan = 0;
			proc->state = RUNNING;
			//release(&ptable.lock);
			cas(&(proc->state),_SLEEPING, RUNNING);
			popcli();
			return -1;
		}

		// Wait for children to exit.  (See wakeup1 call in proc_exit.)
		sched();
	}
}

void 
freeproc(struct proc *p)
{
	if (!p || p->state != _ZOMBIE)
		panic("freeproc not zombie");
	kfree(p->kstack);
	p->kstack = 0;
	freevm(p->pgdir);
	p->killed = 0;
	p->chan = 0;
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
	struct proc *p;

	for(;;){
		// Enable interrupts on this processor.
		sti();

		// Loop over process table looking for process to run.
		//acquire(&ptable.lock);
		pushcli();
		int flag = 0;
		for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
			//if(p->state != RUNNABLE)
			if(cas(&(p->state), RUNNABLE, _RUNNING)){
				flag = 1;
				break;
			}
		}
		if (flag == 0){
			popcli();
			continue;
		}
		// Switch to chosen process.  It is the process's job
		// to release ptable.lock and then reacquire it
		// before jumping back to us.
		proc = p;
		switchuvm(p);
		cas(&(p->state), _RUNNING, RUNNING);
		//p->state = RUNNING;
		swtch(&cpu->scheduler, proc->context);

		cas(&(p->state),_SLEEPING, SLEEPING);
		cas(&(p->state),_RUNNABLE, RUNNABLE);

		switchkvm();

		// Process is done running for now.
		// It should have changed its p->state before coming back.
		proc = 0;
		if (p->state == _ZOMBIE){
			freeproc(p);
			cas(&(p->state),_ZOMBIE, ZOMBIE);
		}
		popcli();
		//}
		//release(&ptable.lock);

	}
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state.
void
sched(void)
{
	int intena;

	//if(!holding(&ptable.lock))
	//	panic("sched ptable.lock");
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
	//acquire(&ptable.lock);  //DOC: yieldlock
	//proc->state = RUNNABLE;
	pushcli();
	cas(&(proc->state),RUNNING, _RUNNABLE);
	sched();
	popcli();
	//release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
	static int first = 1;
	// Still holding ptable.lock from scheduler.
	//release(&ptable.lock);
	popcli();

	if (first) {
		// Some initialization functions must be run in the context
		// of a regular process (e.g., they call sleep), and thus cannot
		// be run from main().
		first = 0;
		initlog();
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
	//if(lk != &ptable.lock){  //DOC: sleeplock0
	//	acquire(&ptable.lock);  //DOC: sleeplock1
	//	release(lk);
	//}

	// Go to sleep.
	proc->chan = (int)chan;
	//TODO
	pushcli();
	cas(&(proc->state),RUNNING, _SLEEPING);
	//proc->state = SLEEPING;
	release(lk);
	sched();

	acquire(lk);
	popcli();
	// Reacquire original lock.
	//if(lk != &ptable.lock){  //DOC: sleeplock2
	//	release(&ptable.lock);
	//	acquire(lk);
	//}
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
	struct proc *p;

	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if(p->state == SLEEPING && p->chan == (int)chan){
			// Tidy up.
			//TODO
			if(cas(&(p->state),SLEEPING, RUNNABLE)){
				p->chan = 0;
			}
			//p->chan = 0;
			//p->state = RUNNABLE;
		}
	}
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
	//acquire(&ptable.lock);
	pushcli();
	wakeup1(chan);
	popcli();
	//release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
	struct proc *p;

	//acquire(&ptable.lock);
	pushcli();
	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if(p->pid == pid){
			p->killed = 1;
			// Wake process from sleep if necessary.
			while (p->state == _SLEEPING);
			//if(p->state == SLEEPING)
			//p->state = RUNNABLE;
			cas(&(p->state),SLEEPING,RUNNABLE);
			//release(&ptable.lock);
			popcli();
			return 0;
		}
	}
	//release(&ptable.lock);
	popcli();
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
		cprintf("%d %s %s", p->pid, state, p->name);
		while(p->state == _SLEEPING);
		if(p->state == SLEEPING){
			getcallerpcs((uint*)p->context->ebp+2, pc);
			for(i=0; i<10 && pc[i] != 0; i++) {
				cprintf(" %p", pc[i]);
			}
			cprintf("\n");
		}
	}
}

void sigret(){
	if(proc == 0) return;
	*(proc->tf) = proc->tfRep;
	cas(&proc->busy, 1, 0);
}

int push(struct cstack *cstack, int sender_pid, int recepient_pid, int value){
	int i;
	for(i = 0; i < 10; i++){
		if (cas(&(cstack->frame[i].used),0,1)) break;
	}
	if (i == 10) {
		return 0;
	}
	cstack->frame[i].sender_pid = sender_pid;
	cstack->frame[i].recepient_pid = recepient_pid;
	cstack->frame[i].value = value;
	struct cstackframe * replica;
	do {
		replica = cstack->head;
	} while (!cas((int *)&(cstack->head),(int)replica,(int)&(cstack->frame[i])));
	cstack->frame[i].next = replica;
	return 1;
}

struct cstackframe *pop(struct cstack *cstack){
	//TODO delete old code
	//while(cstack->head->used == 2);
	//if (!cas(&(cstack->head->used), 1, 2)) return 0;
	if(cstack->head->used == 0) return 0;
	struct cstackframe* ans;
	do{
		ans = cstack->head;
	}
	while(!cas((int *)&(cstack->head), (int)ans, (int)cstack->head->next));
	//cstack->head = cstack->head->next;
	//cstack->head->used = 0;
	ans->used = 0;
	return ans;
}


sig_handler sigset(sig_handler newSig){
	sig_handler replica;
	//TODO delete next two lines
	int pid = proc->pid;
	if(pid);
	do {
		replica = proc->sighandler;
	} while (!cas((int *)&(proc->sighandler),(int)replica,(int)newSig));
	return replica;
}

int sigsend(int dest_pid, int value){
	struct proc *p;
	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if (p->pid == dest_pid && p->state != UNUSED) break;
	}
	if (p == &ptable.proc[NPROC]) return -1;
	int ans = push(&(p->pending_signals), proc->pid, dest_pid, value) -1;
	wakeup((int *) (-1*(int)p));
	return ans;
}

int sigpause(){
	if(proc){
		if(proc->pending_signals.head != 0) return 0;
	}
	for(;;){
		//go to sleep
		proc->chan = -1 * (int)proc;
		//proc->state = SLEEPING;
		cas(&(proc->state),RUNNING,_SLEEPING);
		// wakeup condition
		if(proc->pending_signals.head->used == 1){
			proc->chan = 0;
			//proc->state = RUNNING;
			cas(&(proc->state),_SLEEPING,RUNNING);
			cas(&(proc->state),SLEEPING,RUNNING);
			break;
		}
		//		cprintf("bla bla bla");
		//acquire(&ptable.lock);
		pushcli();
		sched();
		popcli();
		//release(&ptable.lock);
	}
	return 0;
}

extern void sigint(void);
extern void sigintend(void);

void
foo(struct trapframe *tf)
{
	if(proc && tf){
		if(((tf->cs) & 3) !=3)
			return;
		int size;
		int value;
		int retAdd;
		int pid;
		struct cstackframe *poped;
		if(!cas(&proc->busy,0,1))
			return;
		poped = pop(&proc->pending_signals);
		if(poped!=0){
			if(proc->sighandler == (sig_handler)-1){
				poped->used=0;
				proc->busy = 0;
				return;
			}
			proc->tfRep = *(tf);
			size = sigintend-sigint ;
			retAdd = tf->esp-size;
			tf->esp-=size;
			memmove((uint*)tf->esp,sigint,size);
			value = poped->value;
			pid = poped->sender_pid;
			poped->used = 0;
			tf->esp-=4;
			// push signals handler parms
			*((int*)tf->esp) = value;  //push signal value to stack
			tf->esp-=sizeof(int);
			*((int*)tf->esp) = pid;	//push sender pid to stack
			tf->esp-=sizeof(int);
			*((int*)tf->esp) = retAdd; //fix return add
			tf->eip = (uint)proc->sighandler; //eip points to signals handler
		}
		else{
			proc->busy= 0;
		}
	}
}






