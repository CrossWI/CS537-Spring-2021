#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
int main(int argc, char *argv[]) {
    if (argc != 2) {
        // incorrect number of arguments
        exit();
    }
    int sleepticks = atoi(argv[1]);
    if (sleepticks <= 0) {
        // argv[1] can't be coverted
        // printf(2, "sleepticks is negative\n");
        exit();
    }
    // printf(1, "sleeping for %d ticks\n", sleepticks);
    sleep(sleepticks);
    // printf(1, "slept for %d ticks\n", sleepticks);
	
    int i = 0;
    int j = 0;
    while (i < 800000000) {
        j += i * j + 1;
        i++;
    }
    // printf(1, "loop done\n");
    exit();
}
