#include "types.h"
#include "user.h"


int main(int argc, char *argv[])
{
	int counter;
	int pid;
	int childPID;
	int priority;
	int * childPrio = 0;
	int * retime = 0;
	int * stime = 0;
	int * rutime = 0;
	int *startTime = 0;
	int terminationTime;
for (counter = 0 ; counter < 20; ++counter) {
	if((pid = fork()) < 0) {
		//error
		exit();
	}
	if(pid == 0) { //Child
		childPID = getpid();
		priority = (childPID % 3) + 1;
		set_prio(priority);
		int i;
		int j;
		for (i = 0; i < 100; ++i)
		{
			for(j = 0; j < 1000000; ++j)
			{
				//dummy loop
			}
		}
		exit();
	}

}
for(counter = 0; counter < 20; ++counter) {
	pid = wait3(retime, rutime, stime, startTime, childPrio);
	terminationTime = *startTime + *retime + *rutime + *stime;
	if (terminationTime){};
	printf(1,"PID = %d, Termination Time = %d, Priority = %d\n", pid, uptime(), *childPrio) ;
}
exit();

}
