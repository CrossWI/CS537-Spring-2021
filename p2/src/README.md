Additions made to the xv6 in p2:
*Makefile*: Changed CPUs from 2 to 1
			Changed -O2 flag to -Og
*Syscalls.c*: User program that uses two new system calls
*User.h*: Added system calls to header
*usys.S*: Added macros for new system calls
*Syscall.h*: Added numbers for new system calls
*Defs.h*: Declared new system calls to the kernel
*Proc.h*: Added counters for number of system calls and number of good system calls
*Proc.c*: Implemented getnumsyscalls() and getnumsyscallsgood()
		  Initialized proc counters