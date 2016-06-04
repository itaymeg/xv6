#include "types.h"
#include "stat.h"
#include "user.h"

#define LOOP 20


int
main(int argc, char *argv[])
{
	void * pages[LOOP];

	int firstFork;
	int secondFork;
	int i, j;
	for (i = 0; i < LOOP; i++) {
		pages[i]=sbrk(4096);
	}
	for (i = 0; i < LOOP; i++) {
		*(int *)pages[i] = i;
	}
	firstFork = fork();
	if (firstFork != 0){
		for (i = 0; i < 5; i++) {
			*(int *)pages[i] = i;
		}
		for (i = 0; i < 5; i++) {
			*(int *)pages[i] = i;
		}
		for (i = 0; i < 5; i++) {
			*(int *)pages[i] = i;
		}
		for (i = 0; i < 5; i++) {
			*(int *)pages[i] = i;
		}
	}
	for (i = 5; i < 15; i++) {
		*(int *)pages[i] = i;
	}
	secondFork = fork();
	for (j = 0; j < 5; j++){
		for (i = 0; i < 5; i++) {
			//printf(1,"accessing memory: pages[%d] = %d\n", i,*((int *)pages[i]));
			if ((int *)pages[i]);
		}
	}

	if(firstFork > 0)
		wait();

	if (secondFork > 0)
		wait();

	printf(1,"End of Test\n");
	exit();
}

