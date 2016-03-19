#include "types.h"
#include "user.h"
#ifndef MAX_HISTORY
#define MAX_HISTORY 16
#endif

int
main(int argc, char** argv) {
	char * buf = malloc(128);
	char * idBuf = malloc(2);
	idBuf[1] = ' ';
	int i,res;
	for (i = 0; i < MAX_HISTORY; ++i) {
		res = history(buf,i);
		if(res < 0) {
			break;
		}
		idBuf[0] = (char) (i + 48);
		write(1,idBuf,2);
		write(1,buf,strlen(buf));
		write(1,"\n",1);
	}
	exit();
}
