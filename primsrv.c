#include "user.h"

struct worker {
	int pid;
	int used;
};

int isPrime(int num){
	int i;
	for(i = 2; i < (num / 2); i++){
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

struct worker findWorker(struct worker * workers,int size) {
	int i;
	for(i=0; i < size; ++i) {
		if(workers[i].used == 0) {
			return workers[i];
		}
	}
	struct worker ret;
	ret.used = 999;
	return ret;
}
void exitWorkers(struct worker * workers,int size) {
	int i;
	for (i=0; i< size; ++i) {
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
	} else {
		int nextPrime = getNextPrime(value);
		printf(1,"worker %d returned %d as a result for %d", pid, nextPrime, value);
	}
}

int main(int argc, char** argv) {
	int n = atoi(argv[1]);
	int i;
	int pid;
	int child = 0;
	struct worker workers[n];
	printf(1,"workers pids:\n");
	for(i=0; i< n; i++){
		workers[i].used = 0;
		if((pid = fork()) > 0) {
			workers[i].pid = pid;
			printf(1,"%d\n",pid);
		} else if ( pid < 0) {
			printf(1,"fork failed!");
		} else {
			child = 1;
			sig_handler handler = &childHandler;
			sigset(handler);
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
				exitWorkers(workers,n);
				printf(1,"primesrv exit\n");
				exit();
			}
			struct worker availWorker = findWorker(workers,n);
			if(availWorker.used == 999) {
				printf(1,"no idle workers\n");
			} else {
				availWorker.used = 1;
				sigsend(availWorker.pid,enteredNum);
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
