/**
 * CS537 SP2021 - P1: Unix Utilities
 * 
 * Build the utility my-look which is
 * a varient of the UNIX utility look
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

#define BUF_SIZE 128 /** Line input size */

static bool use_stdin = true;
static char *fname = NULL;
static char *prefix = NULL;

/**
 * Parse the arguments passed in the command-line.
 * Accounts for three valid optional arguments as well
 * as required prefix argument
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
                printf("my-look from CS537 Spring 2021\n");
                exit(0);
            case 'h':
                printf("Usage: ./my-look [-Vh] [-f <filename>] <prefix>\n");
                exit(0);
            case 'f':
                use_stdin = false;
                fname = optarg;
                break;
            default:
                printf("my-look: invalid command line\n");
                exit(1);
        }
    }

    /** Final argument must be prefix */
    if (optind == argc - 1) {
        prefix = argv[optind];
    } else {
        printf("my-look: invalid command line\n");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    /** Ensure at least one arg is passed */
    if (argc < 1) {
        printf("my-look: invalid command line\n");
        exit(1);
    }

    /** Parse cmd-line args */
    parse_args(argc, argv);

    /** Open the file */
    FILE *fp;
    if (!use_stdin) {
        fp = fopen(fname, "r");
        if (fp == NULL) {
            printf("my-look: cannot open file\n");
            exit(1);
        }
    } else {
        fp = stdin;
    }

    /** Create a buffer array of line size */
    char buf[BUF_SIZE];

    /**
     * Try to read a line and copy its prefix
     * to stdout if it matches the requested 
     * prefix
     */
    while (fgets(buf, BUF_SIZE, fp) != NULL) {
        int len = strlen(prefix);
        assert(len > 0);

        if (strncasecmp(prefix, buf, len) == 0) {
            printf("%s", buf);
        }
    }

    /** Close the file */
    if (!use_stdin) {
        fclose(fp);
    }

    return 0;
}
