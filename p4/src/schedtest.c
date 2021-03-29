#include "user.h"
#include "pstat.h"

int main(int argc, char **argv) {
    // takes exactly 5 arguments
    if(argc != 6){
        printf(2, "We need exactly 6 arguments.\n");
        exit();
    }

	// set values from command line
    int sliceA = atoi(argv[1]);
    char *sleepA = argv[2];
    int sliceB = atoi(argv[3]);
    char *sleepB = argv[4];
    int sleepParent = atoi(argv[5]);
	struct pstat ps;

	// comptick values for A and B
	int compticksA = 0;
	int compticksB = 0;

	int childA = fork2(sliceA);  // fork child
	int childB = fork2(sliceB);

	if (childA == 0)
	{ // check if the process is a child
		char *args[] = {"loop", sleepA, 0};
        exec("loop", args);
	}

    if (childB == 0){  // check if the process is a child
        char *args[] = {"loop", sleepB, 0};
        exec("loop", args);
    }
    sleep(sleepParent); // sleep the parent

    if (getpinfo(&ps) == 0) { // pstat retrieval success
		// iterate through the pstat proc to find compticks for children
        for (int i = 0; i < NPROC; i++) {
            if (childA == ps.pid[i]) {
                compticksA = ps.compticks[i];
            }
            if (childB == ps.pid[i]) {
                compticksB = ps.compticks[i];
            }
        }
        printf(1, "%d %d\n", compticksA, compticksB); 
    }
	// wait for two loops before exiting
    wait();
    wait();
    exit(); 
}