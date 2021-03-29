#include "user.h"
#include "pstat.h"

//struct pstat* cur_stat; // process statistics
int pidA = -1;
int pidB = -1;

static int fork_child(int slice, char *sleepT) {
    int pid = fork2(slice);
    int child_pid = -1;

    if (pid != -1) {
        if (pid == 0) {  // child
            child_pid = getpid();
            // args to exec
            char *args[2];
            args[0] = "loop";
            args[1] = sleepT;
            exec(args[0], args);

            // exec failed
            printf(2, "exec failed\n");
            exit();
        } else {  // parent
            wait();
        }

    } else {  // fork failed
        printf(2, "fork failed\n");
    }

    return child_pid;
}

int main(int argc, char **argv) {
    // takes exactly 5 arguments
    if(argc != 6){
        printf(2, "We need exactly 6 arguments.\n");
        exit();
    }

    struct pstat stat;
    // get the args
    int sliceA = atoi(argv[1]);
    char* sleepA = argv[2];
    int sliceB = atoi(argv[3]);
    char* sleepB = argv[4];
    int sleepParent = atoi(argv[5]);

    
    // create 2 child processes
    pidA = fork_child(sliceA, sleepA);
    pidB = fork_child(sliceB, sleepB);

    // not sure where to put 
    sleep(sleepParent);

    // cur_stat = (struct pstat*) malloc(sizeof(struct pstat*));
    int compticksA = -1;
    int compticksB = -1;

    if (getpinfo(&stat) == 0) {  // success
        // get the info for the process, given pid
        for (int i = 0; i < NPROC; i++) {
            if (pidA == stat.pid[i]) {
                compticksA = stat.compticks[i];
            }
            if (pidB == stat.pid[i]) {
                compticksB = stat.compticks[i];
            }
        }
    }
    else {
        printf(2, "get info failed.\n");
    }

    printf(1, "%d %d\n", compticksA, compticksB);
    
    exit();
}