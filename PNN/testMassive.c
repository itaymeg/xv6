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
    
    pid1=fork();
    
    int i;
    for (i=0; i<n; i++) {
        printf(1,"$$$$$$$$$$$$$$$$$         sbrk loop   %d  before         pid = %d \n",i,getpid());
        pages[i]=sbrk(4096);
        printf(1,"$$$$$$$$$$$$$$$$$         sbrk loop   %d   0x%x after    pid=%d\n",i,(int)pages[i],getpid());
    }
    
       printf(1,"--------------------------------------------------------$$$$$$$$$$$$$$$$$$$$$$----------------------------------AFTER \n");
    
    for (i=0; i<n; i++) {
        *(int *)pages[i] = i;
        printf(1,"\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$         use loop   %d   pid= %d  \n",i,getpid());
    }
      printf(1,"--------------------------------------------------BEFORE SECOND FORK\n");
    pid2=fork();
    
    printf(1,"-------------------------------------------AFTER SECOND FORK\n");
    
    for (i=0; i<n; i++) {
       printf(1,"\n\n\n------------------TEST MASSIVE %d   pid %d\n\n\n", *((int *)pages[i]),getpid() );
    }
     printf(1,"\n\n\n-----------------------------------------------------BEFORE WAIT\n\n\n\n");
   
    if(pid1>0)
        wait();
    
    if (pid2>0)
        wait();

  
   printf(1,"--------------------------AFTER WAIT\n");
 
    exit();
}
 
