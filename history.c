#include "types.h"
#include "user.h"
#ifndef MAX_HISTORY
#define MAX_HISTORY 16
#endif

int
main(int argc, char** argv) {
	char * buf = malloc(128);
	char * idBuf = malloc(3);
	idBuf[1] = ' ';
	idBuf[2] = ' ';
	int i,res;
	for (i = 0; i < MAX_HISTORY; ++i) {
		res = history(buf,i);
		if(res < 0) {
			break;
		}
		if(i < 10) {
			idBuf[0] = (char) (i + 48);
		} else {
			int imod = i % 10;
			idBuf[0] = (char) (1 + 48);
			idBuf[1] = (char) (imod + 48);
		}
		write(1,idBuf,3);
		write(1,buf,strlen(buf));
		write(1,"\n",1);
		int j;
		for(j = 0; j < 128; ++j){
			buf[j] = 0;
		}
	}
	free(buf);
	free(idBuf);
	exit();
}
