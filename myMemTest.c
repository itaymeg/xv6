#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

#define PGSIZE 4096
#define LOOP 27

char* m1[LOOP];

volatile int
main(int argc, char *argv[])
{

	int i,j;
//	int pid;
	//creating 'COUNT' pages
	for (i = 0; i < LOOP ; ++i)
	{
//		if (i% 2 == 0){
//			if ((pid = fork()) == 0) break;
//		}
		m1[i] = sbrk(PGSIZE);
		printf(1, "allocated page #%x at address: %x\n", i, m1[i]);
	}


	//using all pages
	for ( i = 0; i < LOOP; ++i)
	{
		for ( j = 0; j < PGSIZE; ++j)
		{
			m1[i][j] = 0;
		}
	}

	printf(1,"Finished Successfuly!!!\n");

	exit();
	return 0;
}
