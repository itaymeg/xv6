#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_yield(void){
	yield();
	return 0;
}

int
sys_wait(void)
{
  return wait();
}

int
sys_wait2(void){
	int retimeAdd = 0;
	int rutimeAdd = 0;
	int stimeAdd = 0;
	int res = 0;
	if(argint(0, &retimeAdd) < 0 || argint(1, &rutimeAdd) < 0 || argint(2, &stimeAdd) < 0){
		return -1;
	}
	res = wait2((int*) retimeAdd,(int*) rutimeAdd,(int*) stimeAdd);
	return res;
}


int
sys_start_time(void){
	int stimeAdd = 0;
	if (argint(0, &stimeAdd) < 0){
		return -1;
	}
	*(int *)stimeAdd = proc->ctime;
	return 0;
}

int
sys_set_prio(void){
	int prio;
	if (argint(0, &prio) < 0 || prio < 0 || prio > 3){
		return -1;
	}
	proc->priority = prio;
	return 0;
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
