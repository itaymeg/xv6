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
 
 
// #include "types.h"
// #include "stat.h"
// #include "user.h"
// #include "fs.h"
// #include "mmu.h"
// 
// #define NUM_OF_PAGES 20
// 
// int
// main(int argc, char *argv[])
// {
//   
//   int i, j;
//   printf(1, "*** ENTERED myMemTest ***\n");
//   //int address = *sbrk(NUM_OF_PAGES*PGSIZE);
//   //printf(1, "Address =  %x\n", address);
//   int pid = fork(); //basically we fork cause we don't want the son process to be shell, since shell & init shouldn't
//   // be changed by the paging framework.
//   if (pid) {
//     wait();
//     exit();
//   }
//   char* tempPtr = malloc(NUM_OF_PAGES*PGSIZE); 
// 
// //   for(i=1, j=0; i<(NUM_OF_PAGES-1)*PGSIZE; i += PGSIZE, j++) { 
// //     printf(1, "Setting memory address %d (page index %d) to %d\n", tempPtr+i/4, (int)(tempPtr+i/4)/PGSIZE, j);  
// //     *(tempPtr+i/4) = j;
// //   }
// //   
// //   for(i=1; i<(NUM_OF_PAGES-1)*PGSIZE; i += PGSIZE) {
// //     printf(1, "Value at memory address %d (page index %d): %d\n", tempPtr+i/4, (int)(tempPtr+i/4)/PGSIZE, *(tempPtr+i/4));  
// //   }
// //   
// 
//     for(i=1, j=0; i<NUM_OF_PAGES*PGSIZE; i += PGSIZE, j++) { 
//     printf(1, "Setting memory address %d (page index %d) to %d\n", tempPtr+i, (int)(tempPtr+i)/PGSIZE, j);  
//     *(tempPtr+i) = j;
//   }
//   
//   for(i=1; i<NUM_OF_PAGES*PGSIZE; i += PGSIZE) {
//     printf(1, "Value at memory address %d (page index %d): %d\n", tempPtr+i, (int)(tempPtr+i)/PGSIZE, *(tempPtr+i));  
//   }
//   
//   
//   printf(1, "*** EXIT myMemTest ***\n");
//   
//   exit();
// }