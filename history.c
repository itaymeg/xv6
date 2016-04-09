#include "types.h"
#include "user.h"


int
main(int argc, char** argv) {
	int i;
	int pid;
	for (i = 0; i < 9; i++){
		pid = fork();
		if (pid > 0){
			printf(1, "pid = %d", pid);
		} else {
			exit();
		}
	}
	exit();
}
