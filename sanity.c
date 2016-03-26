
#include "types.h"
#include "user.h"

int main(int argc, char** argv) {
	if (argc > 1){
		int retime = 0, rutime = 0, stime = 0, turnaround = 0;
		int pid, sumCpu, sumScpu, sumIo;
		int cpuRetime = 0, cpuStime = 0, cpuTurnaround = 0;
		int scpuRetime = 0, scpuStime = 0, scpuTurnaround = 0;
		int ioRetime = 0, ioStime = 0, ioTurnaround = 0;
		char* type;
		int n = atoi(argv[1]);
		int i;
		for (i = 0; i < 3 * n; ++i){
			pid = fork();
			if (pid == 0){
				int cpid = getpid();
				if (cpid % 3 == 0){
					int j,k;
					for (j = 0; j < 100; ++j){
						for (k = 0; k < 1000000; ++k){

						}
					}
				}
				else if (cpid % 3 ==1){
					int j,k;
					for (j = 0; j < 100; ++j){
						for (k = 0; k < 1000000; ++k){

						}
						yield();
					}
				}
				else{
					int j;
					for (j = 0; j < 100; ++j){
						sleep(1);
					}
				}
				exit();
			}
			else {
				if (pid != -1){
					if(pid % 3 == 0){
						sumCpu++;
					}
					else if(pid % 3 == 1){
						sumScpu++;
					}
					else{
						sumIo++;
					}
				}
			}
		}

		for (i = 0; i < 3 * n; ++i){
			pid = wait2(&retime, &rutime, &stime);
			if (pid != -1){
				if(pid % 3 == 0){
					type = "CPU";
					cpuStime = cpuStime + stime;
					cpuRetime = cpuRetime + retime;
					turnaround = retime + rutime + stime;
					cpuTurnaround = cpuTurnaround + turnaround;
				}
				else if(pid % 3 == 1){
					type = "S-CPU";
					scpuStime = scpuStime + stime;
					scpuRetime = scpuRetime + retime;
					turnaround = retime + rutime + stime;
					scpuTurnaround = scpuTurnaround + turnaround;
				}
				else{
					type = "I/O";
					ioStime = ioStime + stime;
					ioRetime = ioRetime + retime;
					turnaround = retime + rutime + stime;
					ioTurnaround = ioTurnaround + turnaround;
				}
			}
			printf(2,"Type = %s, ", type);
			printf(2,"PID = %d, ", pid);
			printf(2,"Wait Time = %d, ", retime);
			printf(2,"Run Time = %d, ", rutime);
			printf(2,"I/O Time = %d\n", stime);

		}




		printf(2,"CPU:\n");
		printf(2,"Average Sleep Time      = %d\n", cpuStime/sumCpu);
		printf(2,"Average Ready Time      = %d\n", cpuRetime/sumCpu);
		printf(2,"Average Turnaround Time = %d\n", cpuTurnaround/sumCpu);

		printf(2,"S-CPU:\n");
		printf(2,"Average Sleep Time      = %d\n", scpuStime/sumScpu);
		printf(2,"Average Ready Time      = %d\n", scpuRetime/sumScpu);
		printf(2,"Average Turnaround Time = %d\n", scpuTurnaround/sumScpu);

		printf(2,"I/O:\n");
		printf(2,"Average Sleep Time      = %d\n", ioStime/sumIo);
		printf(2,"Average Ready Time      = %d\n", ioRetime/sumIo);
		printf(2,"Average Turnaround Time = %d\n", ioTurnaround/sumIo);
	}
	exit();
}
