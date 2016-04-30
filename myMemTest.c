#include "types.h"
#include "stat.h"
#include "user.h"

#define LOOP 20
#define SIZE 50

int main(){
	int i, j;
	for (i = 0; i < LOOP; i++){
		int arr[SIZE];
		for (j = 0; j < SIZE; j++){
			arr[j] = j;
		}
		for (j = 0; j < SIZE; j++){
			if (arr[j] % 2 == 0){
				arr[j] = 1;
			}
		}
		sleep(1);
	}
	exit();
}




