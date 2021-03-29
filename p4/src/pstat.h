#ifndef _PSTAT_H_
#define _PSTAT_H_

#include "param.h"

struct pstat {
  int inuse[64]; // whether this slot of the process table is in use (1 or 0)
  int pid[64]; // PID of each process
  int timeslice[64]; // number of base ticks this process can run in a timeslice
  int compticks[64]; // number of compensation ticks this process has used
  int schedticks[64];  // total number of timer ticks this process has been scheduled
  int sleepticks[64]; // number of ticks during which this process was blocked
  int switches[64];  // total num times this process has been scheduled
};
#endif