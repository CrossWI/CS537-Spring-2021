# Build by running 'make mysh' or simply 'make'
# Clean by running 'make clean'

# A couple standard variables:
# C Compiler
CC=gcc
# C Flags
CFLAGS=-Wall -g

# Main build command
mysh: mysh.c
	$(CC) $(CFLAGS) -o mysh mysh.c

# .PHONY means this command does not actually produce an output file
.PHONY: clean
clean:
	rm -rf mysh
