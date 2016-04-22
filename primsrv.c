#include "user.h"

struct worker {
	int pid;
	int used;
	int sigValue;
};

struct worker * workers;
int workersCount;

int isPrime(int num){
	int i;
	for(i = 2; i <= (num / 2); i++){
		if (num % i == 0)
			return 0;
	}
	return 1;
}

int getNextPrime(int n){
	int found = 0;
	int ans = n + 1;
	while(found == 0){
		if(isPrime(ans) == 1){
			found = 1;
		}
		else{
			ans++;
		}
	}
	return ans;
}

int findWorker() {
	int i;
	for(i=0; i < workersCount; ++i) {
		if(workers[i].used == 0) {
			workers[i].used = 1;
			return i;
		}
	}
	return -1;
}
void exitWorkers() {
	int i;
	for (i=0; i< workersCount; ++i) {
		int ans = -1;
		while (ans == -1) {
			ans = sigsend(workers[i].pid,0);
		}
	}
}
void childHandler(int pid,int value) {
	if(value == 0) {
		printf(1,"worker %d exit",pid);
		exit();
	}
		int nextPrime = getNextPrime(value);
		//printf(1,"worker %d returned %d as a result for %d\n", pid, nextPrime, value);
		sigsend(pid,nextPrime);
}
void parentHandler(int pid,int value) {
	int i;
	for(i = 0; i < workersCount; ++i) {
		if (pid == workers[i].pid)
		{
			workers[i].used = 0;
			printf(1,"worker %d returned %d as a result for %d\n", pid, value, workers[i].sigValue);	
			break;
		}
	}
}
void waitExitWorkers() {
	int i;
	for (i=0; i< workersCount; ++i) {
		wait();
	}
}

int main(int argc, char** argv) {
	int n = atoi(argv[1]);
	sigset(parentHandler);
	workersCount = n;
	workers= (struct worker *)malloc(workersCount*sizeof(struct worker));
	int i;
	int pid;
	int child = 0;
	printf(1,"workers pids:\n");
	for(i=0; i < workersCount; i++){
		workers[i].used = 0;
		if((pid = fork()) > 0) {
			workers[i].pid = pid;
			printf(1,"%d\n",pid);
		} else if ( pid < 0) {
			printf(1,"fork failed!");
			exit();
		} else {
			child = 1;
			sigset(childHandler);
			break;
		}
	}

	if(child == 0) { //parent
		for (;;) {
			printf(1,"please enter a number: ");
			int enteredNum;
			int legal = 0;
			char buf[128];
			int i;
			for(i =0; i < 128; i++){
				buf[i] = 0;
			}
			read(0, buf, 127);
			if (buf[0] == 48 && buf[1] == 10) {
				legal = 1;
			}
			enteredNum = atoi(buf);
			if(legal == 0 && enteredNum ==0) continue;
			if(enteredNum == 0) {
				exitWorkers();
				waitExitWorkers();
				free(workers);
				printf(1,"primesrv exit\n");
				exit();
			}
			int availWorker = findWorker();
			if(availWorker == -1) {
				printf(1,"no idle workers\n");
			} else {
				workers[availWorker].sigValue = enteredNum;
				sigsend(workers[availWorker].pid,enteredNum);
			}
		}
		exit();
	} else if(child == 1) { // child
		for(;;) {
			sigpause();
		}
		exit();
	}
	exit();
}
