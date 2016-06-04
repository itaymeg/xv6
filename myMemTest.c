#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
    int n=20;
    void * pages[n];

    int pid1;
    int pid2;

    printf(1,"------- BEFORE FIRST FORK -------\n");
    pid1=fork();
    printf(1,"------- AFTER FIRST FORK -------\n");

    int i;
    for (i=0; i<n; i++) {
        pages[i]=sbrk(4096);
        printf(1,"allocated page %d at address 0x%x for process %d\n",i,(int)pages[i],getpid());
    }

       printf(1,"process %d finished allocating pages\n",getpid());

       printf(1,"process %d access it's memory\n",getpid());
    for (i=0; i<n; i++) {
        *(int *)pages[i] = i;
    }
    printf(1,"process %d finished accessing it's memory\n",getpid());

    printf(1,"------- BEFORE SECOND FORK -------\n");
    pid2=fork();
    printf(1,"------- AFTER SECOND FORK -------\n");

    for (i=0; i<n; i++) {
       printf(1," process %d is accessing it's memory: pages[%d] = %d\n",getpid(), i,*((int *)pages[i]) );
    }

    if(pid1>0)
        wait();

    if (pid2>0)
        wait();

   printf(1,"Finished Successfuly!!!\n");
    exit();
}



//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//
//
//#include "param.h"
//#include "types.h"
//#include "stat.h"
//#include "user.h"
//#include "fs.h"
//#include "fcntl.h"
//#include "syscall.h"
//#include "traps.h"
//#include "memlayout.h"
//
//#define PGSIZE 4096
//#define LOOP 27
//
//char* m1[LOOP];
//
//volatile int
//main(int argc, char *argv[])
//{
//
//	int i,j;
//	int k = 0;
//
//
////	int pid;
//	//creating 'COUNT' pages
//	for (i = 0; i < LOOP ; ++i)
//	{
////		if (i% 2 == 0){
////			if ((pid = fork()) == 0) break;
////		}
//		m1[i] = sbrk(PGSIZE);
//		printf(1, "allocated page #%x at address: %x\n", i, m1[i]);
//	}
//
//
//	//using all pages
//	for ( i = 0; i < LOOP; ++i)
//	{
//		for ( j = 0; j < PGSIZE; ++j)
//		{
//			m1[i][j] = 0;
//			test:
//			if (k == 0) {printf(1,"!!!!!!!!!!%x\n", &&test); k++;}
//		}
//	}
//
//
//	printf(1,"Finished Successfuly!!!\n");
//
//	exit();
//	return 0;
//}
