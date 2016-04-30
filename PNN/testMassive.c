#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	void * p;
  p = malloc(4096);
  *(int *)p = 1;
  return 0;
}
