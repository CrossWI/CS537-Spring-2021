#include "types.h"
#include "stat.h"
#include "user.h"

/*
 * User level program to test two new system calls:
 * getnumsyscalls() and getnumsyscallsgood()
 */
int
main(int argc, char *argv[])
{
  int numcalls = atoi(argv[1]);
  int numcallsgood = atoi(argv[2]);
  int pid = getpid();

  for (int i = 0; i < numcallsgood - 1; i++)
  {
    getpid();
  }

  for (int j = 0; j < numcalls - numcallsgood; j++)
  {
    kill(1111);
  }

  int calls = getnumsyscalls(pid);
  int callsgood = getnumsyscallsgood(pid);

  printf(1, "%d %d\n", calls, callsgood);

  exit();
}
