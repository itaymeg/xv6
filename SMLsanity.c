#include "types.h"
#include "user.h"


int main(int argc, char *argv[])
{
	int i, j, counter;
	int retime = 0, rutime = 0, stime = 0, ctime = 0;

	int terminationTime = 0;

	for (counter=0; counter < 20; counter++){
		int priority;
		int pid = fork();
		if(pid == 0){ // Child
			priority = (getpid() % 3) + 1;
			set_prio(priority);
			for(i = 0; i < 100; i++){
				for(j = 0; j < 1000000; j++){
				}
			}

			exit();

		}
		else{ // Parent
			int childPID;
			childPID = wait2(&retime, &rutime, &stime);
			start_time(&ctime);
			terminationTime = ctime + retime + rutime + stime;
			priority = (childPID % 3) +1;
			if (childPID != -1){
				printf(2, "PID = %d, Termination Time = %d, Priority = %d\n", childPID, terminationTime, priority);

			}
		}
	}


	return 0;
}
