//#include "types.h"
//#include "stat.h"
//#include "user.h"
//
//#define LOOP 20
//#define SIZE 50
//
//int main(){
////	int i, j;
////	for (i = 0; i < LOOP; i++){
////		int arr[SIZE];
////		for (j = 0; j < SIZE; j++){
////			arr[j] = j;
////		}
////		for (j = 0; j < SIZE; j++){
////			if (arr[j] % 2 == 0){
////				arr[j] = 1;
////			}
////		}
////		sleep(100);
////	}
//	void * p;
//	p = malloc(4096);
//	*(int *)p = 1;
//	return 0;
//
//	exit();
//}
//
//
//
//

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
#define COUNT 20

char* m1[COUNT];

volatile int
main(int argc, char *argv[])
{

int i,j;

//creating 'COUNT' pages
for (i = 0; i < COUNT ; ++i)
{
m1[i] = sbrk(PGSIZE);
printf(1, "allocated page #%d at address: %x\n", i, m1[i]);
}

//using all pages
for ( i = 0; i < COUNT; ++i)
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
