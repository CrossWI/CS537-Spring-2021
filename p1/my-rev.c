/**
 * CS537 SP2021 - P1: Unix Utilities
 * 
 * Build the utility my-rev which is
 * a varient of the UNIX utility rev
 * 
 * External help: Used DIS 315 my-cat.c
 * to help with creation
 *
 * Copyright 2021 Cameron Cross
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define BUF_SIZE 100 /** Line input size */

static char *fname = NULL;
static bool use_stdin = true;

/**
 * Parse the arguments passed in the command-line.
 * Accounts for three valid optional arguments.
 */
static void parse_args(int argc, char *argv[]) {
    /** Possible option: 
     * -V
     * -h
     * -f with a filename
     */
    int opt;
    while ((opt = getopt(argc, argv, "Vhf:")) != -1) {
        switch (opt) {
            case 'V':
                printf("my-rev from CS537 Spring 2021\n");
                exit(0);
            case 'h':
                printf("Usage: ./my-rev [-Vh] [-f <filename>]\n");
                exit(0);
            case 'f':
                use_stdin = false;
                fname = optarg;
                break;
            default:
                printf("my-rev: invalid command line\n");
                exit(1);
        }
    }

    /** Ensure no other arguments are passed */
    if (optind != argc) {
        printf("my-rev: invalid command line\n");
        exit(1);
    }
}

/**
 * Reverses the input string.
 * Accounts for new-line character, ensuring
 * it remains at the end of the string
 */
char *reverse_string(char *str, int len) {
    char *start = str;
    char *end;

    /** If a newline char is present, then
     * the end of the string is one before the
     * newline char */
    if (str[len - 1] == '\n') {
        end = str + len - 2;
    } else {
        end = str + len - 1;
    }

    /** Swap chars from front of string
     * and back of string until pointers
     * meet each other in the middle */
    while (start < end) {
        char tmp = *start;
        *start++ = *end;
        *end-- = tmp;
    }
    return str;
}

int main(int argc, char *argv[]) {
    /** Ensure at least one arg is passed */
    if (argc < 1) {
        printf("my-rev: invalid command line\n");
        exit(1);
    }

    /** Parse cmd-line args */
    parse_args(argc, argv);

    /** Open the file */
    FILE *fp;
    if (!use_stdin) {
        fp = fopen(fname, "r");
        if (fp == NULL) {
            printf("my-rev: cannot open file\n");
            exit(1);
        }
    } else {
        fp = stdin;
    }

    /** Create a buffer array of line size */
    char buf[BUF_SIZE];

    /**
     * Try to read a line and copy its
     * reverse to stdout
     */
    while (fgets(buf, BUF_SIZE, fp) != NULL) {
        int len = strlen(buf);
        assert(len > 0);
        reverse_string(buf, len);
        printf("%s", buf);
    }

    /** Close the file */
    if (!use_stdin) {
        fclose(fp);
    }

    return 0;
}
