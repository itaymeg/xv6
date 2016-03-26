#include "types.h"
#include "user.h" 


int main(int argc, char *argv[])
{
  int i, j, count;
  int retime = 0, rutime = 0, stime = 0, turnaround = 0;
  int pid, sumCPU, sumSCPU, sumIO;
  int CPUretime = 0, CPUstime = 0, CPUturnaround = 0;
  int SCPUretime = 0, SCPUstime = 0, SCPUturnaround = 0;
  int IOretime = 0, IOstime = 0, IOturnaround = 0;
  int dummy = 0;
  char* type;
  printf(2, "this line is a test before atoi!!");
  int n = atoi(argv[1]);
  for (count=0; count < 3*n;count++){
	  printf(2, "this line is a test before fork!!\n");
    //child
    if(fork() == 0){
    	printf(2, "this line is a test after fork!!\n");
      //CPU process
      if(getpid() % 3 == 0){
	for(i = 0; i < 100; i++){
	  for(j = 0; j < 1000000; j++){
	   //dummy loops 
	    dummy++;
	  }
	}
	//S-CPU process
      }else if(getpid() % 3 == 1){
	for(i = 0; i < 100; i++){
	  for(j = 0; j < 1000000; j++){
	   //dummy loops 
	    dummy++;
	  }
	  yield();
	}
	//IO process
      }else{
	for(i = 0; i < 100; i++){
	  sleep(1);
	}
      }
      
      exit();
    }
    
    //parent
    else{
    	pid = wait2(&retime, &rutime, &stime);
    	if (pid != -1){
    		if(pid % 3 == 0){
    			type = "CPU";
    			CPUstime = CPUstime + stime;
    			CPUretime = CPUretime + retime;
    			turnaround = retime + rutime + stime;
    			CPUturnaround = CPUturnaround + turnaround;
    			sumCPU = sumCPU + 1;
    		}
    		else if(pid % 3 == 1){
    			type = "S-CPU";
    			SCPUstime = SCPUstime + stime;
    			SCPUretime = SCPUretime + retime;
    			turnaround = retime + rutime + stime;
    			SCPUturnaround = SCPUturnaround + turnaround;
    			sumSCPU = sumSCPU + 1;
    		}
    		else{
    			type = "I/O";
    			IOstime = IOstime + stime;
    			IOretime = IOretime + retime;
    			turnaround = retime + rutime + stime;
    			IOturnaround = IOturnaround + turnaround;
    			sumIO = sumIO + 1;
    		}
    		/*
	SUMretime = SUMretime + retime;
	SUMrutime = SUMrutime + rutime;
	SUMstime = SUMstime + stime;*/
    		//printf(2,"process id :%d, process type :%s, waiting time :%d, running time :%d, I/O time :%d \n",pid, type, retime, rutime, stime);
    		printf(2,"process id :%d\n",pid);
    		printf(2,"process type :%s\n",type);
    		printf(2,"waiting time :%d\n",retime);
    		printf(2,"running time :%d\n",rutime);
    		printf(2,"I/O time :%d\n",stime);

    		/*//time for a job to complete.
	turnaround = retime + rutime + stime;
	SUMturnaround = SUMturnaround + turnaround;
	printf(2,"turnaround time :%d\n",turnaround);
	printf(1,"\n");*/

    	}
    }
  }
  printf(2,"CPU average sleep time :%d\n",(CPUstime)/sumCPU);
  printf(2,"CPU average ready time :%d\n",(CPUretime)/sumCPU);
  printf(2,"CPU average turnaround time :%d\n",(CPUturnaround)/sumCPU);

  printf(2,"S-CPU average sleep time :%d\n",(SCPUstime)/sumSCPU);
  printf(2,"S-CPU average ready time :%d\n",(SCPUretime)/sumSCPU);
  printf(2,"S-CPU average turnaround time :%d\n",(SCPUturnaround)/sumSCPU);

  printf(2,"I/O average sleep time :%d\n",(IOstime)/sumIO);
  printf(2,"I/O average ready time :%d\n",(IOretime)/sumIO);
  printf(2,"I/O average turnaround time :%d\n",(IOturnaround)/sumIO);
  
  return 0;
}
