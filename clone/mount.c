#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	if(argc != 3){
		exit();
	}
	if(mount(argv[1],(uint)atoi(argv[2])) != 0)
		printf(1,"mount failed \n");
	exit();
} 
 
