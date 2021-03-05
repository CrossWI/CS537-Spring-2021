#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_CMD_LINE_SIZE 512
#define MAX_LINE_CMDS 100

// error messages
static char *INVALID_CMD_LINE = "Usage: mysh [batch-file]\n";
static char *INVALID_REDIRECT = "Redirection misformatted.\n";
static char *UNALIAS_PARTS_ERR = "unalias: Incorrect number of arguments.\n";
static char *ALIAS_DANGER_ERR = "alias: Too dangerous to alias that.\n";

// command prompt
static char *COMMAND_PROMPT = "mysh> ";

// linked list node structure
typedef struct Alias_Node {
    char *alias;
    char *path[MAX_LINE_CMDS];
    int num;
    struct Alias_Node *next;
    struct Alias_Node *prev;
} Alias_Node;

/**
 * Prints the linked list of alias nodes.
 * Format is: [alias] [path] 
 */
void printList(Alias_Node *head) {
    if (head == NULL) {
        return;
    }
    // iterate through linked list to print
    Alias_Node *temp = NULL;
    temp = head->next;
    while (temp != NULL) {
        char path[MAX_CMD_LINE_SIZE] = "";
        for (int i = 0; i < temp->num; i++) {
            if (i == temp->num - 1) {
                strcat(path, temp->path[i]);
            } else {
                strcat(path, temp->path[i]);
                strcat(path, " ");
            }
        }
        fprintf(stdout, "%s %s\n", temp->alias, path);
        fflush(stdout);
        temp = temp->next;
    }
}

/**
 * Adds the path of a node to its linked list node. 
 * Assigns instruction arguments to an array of strings
 * in node.
 */
void addNodePath(Alias_Node *head, char *path[], int num) {
    for (int i = 0; i < num; i++) {
        head->path[i] = strdup(path[i]);
    }
}

/**
 * Checks if the given node is in the linked list
 */
Alias_Node *checkAlias(Alias_Node *head, char *alias) {
    Alias_Node *temp = head->next;

    while (temp != NULL) {
        if (strcmp(temp->alias, alias) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

/**
 * mysh mode that handles command line inputs
 * from the user.
 */
void interactive_mode() {
    // allocate space to an empty linked list
    Alias_Node *head = malloc(sizeof(Alias_Node));
    Alias_Node *tail = malloc(sizeof(Alias_Node));
    head->next = tail;
    head->prev = NULL;
    tail->next = NULL;
    tail->prev = head;

    // loop until exit or EOF
    while (1) {
        write(STDOUT_FILENO, COMMAND_PROMPT, strlen(COMMAND_PROMPT));
        char userline[MAX_CMD_LINE_SIZE];

        // wait for user input
        if (fgets(userline, MAX_CMD_LINE_SIZE, stdin) == NULL) {
            return;
        } else {
            // execute
            if (strcmp(userline, "exit\n") == 0) {
                return;
            }

            // duplicate user line for manipulation
            char *line = strdup(userline);
            char *redir = strdup(userline);
            char *args[MAX_LINE_CMDS];
            int redir_flag = 0;  // flag for redirect

            // break up userline arguments into array of strings.
            // one argument per string
            args[0] = strtok(line, " \t\n");

            int i = 0;

            while (i <= 100 && args[i] != NULL) {
                args[++i] = strtok(NULL, " \t\n");
            }

            // if user input is empty
            if (args[0] == NULL) {
                free(line);
                free(redir);
                continue;
            }

            // check aliasing
            if (strcmp(args[0], "alias") == 0) {
                if (i < 3) {
                    if (i == 1) {  // print
                        printList(head->next);
                        continue;
                    } else {  // print single alias
                        Alias_Node *print = checkAlias(head->next, args[1]);
                        if (print != NULL) {
                            char path[512] = "";
                            for (int i = 0; i < print->num; i++) {
                                if (i == print->num - 1) {
                                    strcat(path, print->path[i]);
                                } else {
                                    strcat(path, print->path[i]);
                                    strcat(path, " ");
                                }
                            }
                            fprintf(stdout, "%s %s\n", print->alias, path);
                            fflush(stdout);
                        }
                        continue;
                    }
                }

                // check if the alias name is alias, unalias, or exit
                if (strcmp(args[1], "alias") == 0
                        || strcmp(args[1], "unalias") == 0
                        || strcmp(args[1], "exit") == 0) {
                    write(STDERR_FILENO, ALIAS_DANGER_ERR,
                            strlen(ALIAS_DANGER_ERR));
                    continue;
                }

                // copy user line arguments for path into a path array
                char *path[MAX_LINE_CMDS];
                for (int j = 2; j <= i; j++) {
                    if (j <= i) {
                        path[j - 2] = args[j];
                    } else {
                        path[j - 2] = NULL;
                    }
                }

                // check if alias node exists alread.
                // if so, replace its current path
                Alias_Node *exists = checkAlias(head->next, args[1]);
                if (exists != NULL) {
                    addNodePath(exists, path, i - 2);
                    exists->num = i - 2;
                    continue;
                }

                // if not, add new node
                Alias_Node *new_node = malloc(sizeof(Alias_Node));
                new_node->alias = args[1];
                addNodePath(new_node, path, i - 2);
                new_node->prev = tail;
                new_node->num = i - 2;
                tail->next = new_node;
                tail = new_node;
                continue;
            }

            // check unaliasing
            if (strcmp(args[0], "unalias") == 0) {
                if (i > 2 || i < 2) {
                    write(STDERR_FILENO, UNALIAS_PARTS_ERR,
                            strlen(UNALIAS_PARTS_ERR));
                    continue;
                } else {  // delete alias specified
                    Alias_Node *del = checkAlias(head->next, args[1]);
                    if (del == NULL) {
                        continue;
                    } else {
                        if (del->next == NULL) {
                            tail = tail->prev;
                            tail->next = NULL;
                            free(del);
                            del = NULL;
                            continue;
                        } else {
                            del->prev->next = del->next;
                            del->next->prev = del->prev;
                            free(del);
                            del = NULL;
                            continue;
                        }
                    }
                }
            }

            // check redirect
            if (strstr(redir, ">") != NULL) {
                redir_flag = 1;
            }

            char *forward = strchr(redir, '>');
            char *backward = strrchr(redir, '>');
            char *redir_args[MAX_LINE_CMDS];
            char *redir_output[MAX_LINE_CMDS];

            // check for multiple >
            if (forward != backward && redir_flag) {
                write(STDERR_FILENO, INVALID_REDIRECT,
                        strlen(INVALID_REDIRECT));
                free(redir);
                continue;
            }

            // break up command line into arguments and output
            int redir_count = 0;
            char *before_redirect = strtok(redir, ">");
            char *after_redirect = strtok(NULL, ">");

            redir_output[0] = strtok(after_redirect, " \t\n");
            while (redir_count <= 100 && redir_output[redir_count] != NULL) {
                redir_output[++redir_count] = strtok(NULL, " \t\n");
            }

            // make sure there is only one output
            if (redir_count != 1 && redir_flag) {
                write(STDERR_FILENO, INVALID_REDIRECT,
                        strlen(INVALID_REDIRECT));
                free(redir);
                continue;
            }

            int rd_cmd = 0;
            redir_args[0] = strtok(before_redirect, " \t\n");
            while (rd_cmd <= 100 && redir_args[rd_cmd] != NULL) {
                redir_args[++rd_cmd] = strtok(NULL, " \t\n");
            }

            // make sure there is at least one argument
            if (rd_cmd == 0 && redir_flag) {
                write(STDERR_FILENO, INVALID_REDIRECT,
                        strlen(INVALID_REDIRECT));
                free(redir);
                continue;
            }


            // fork
            pid_t pid = fork();

            if (pid != -1) {
                // child
                if (pid == 0) {
                    Alias_Node *alias = checkAlias(head->next, args[0]);
                    if (alias != NULL) {
                        execv(alias->path[0], alias->path);
                        fprintf(stderr, "%s: Command not found.\n",
                                alias->path[0]);
                        fflush(stderr);
                        _exit(1);
                    }

                    if (redir_flag) {
                        int fd = open(redir_output[0], O_WRONLY | O_CREAT
                                | O_TRUNC, 0600);
                        dup2(fd, STDOUT_FILENO);
                        close(fd);


                        execv(redir_args[0], redir_args);
                        fprintf(stderr, "%s: Command not found.\n",
                                redir_args[0]);
                        fflush(stderr);
                        _exit(1);
                    }

                    execv(args[0], args);

                    // if child reached here, exec failed
                    fprintf(stderr, "%s: Command not found.\n", args[0]);
                    fflush(stderr);
                    _exit(1);
                } else {  // parent
                    int status;
                    waitpid(pid, &status, 0);
                }
            } else {  // fork failed
                printf("mysh: fork failed\n");
            }
            free(line);
            free(redir);
        }
    }
}

/**
 * mysh mode that handles command line inputs
 * from a given file.
 */
void batch_mode(char *argv[]) {
    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        fprintf(stderr, "Error: Cannot open file %s.\n", argv[1]);
        fflush(stderr);
        exit(1);
    }

    // creat a buffer array of line size
    char buf[MAX_CMD_LINE_SIZE];

    // allocate empty linked list
    Alias_Node *head = malloc(sizeof(Alias_Node));
    Alias_Node *tail = malloc(sizeof(Alias_Node));
    head->next = tail;
    head->prev = NULL;
    tail->next = NULL;
    tail->prev = head;

    // read file line by line
    while (fgets(buf, MAX_CMD_LINE_SIZE, input) != NULL) {
        if (strlen(buf) > MAX_CMD_LINE_SIZE) {
            write(STDOUT_FILENO, buf, MAX_CMD_LINE_SIZE);
            continue;
        }
        write(STDOUT_FILENO, buf, strlen(buf));

        // execute
        if (strcmp(buf, "exit\n") == 0) {
            break;
        }

        // duplicate buffer line for manipulation
        char *line = strdup(buf);
        char *redir = strdup(buf);
        char *args[MAX_LINE_CMDS];
        int redir_flag = 0;  // flag for redirect

        // break up buffer arguments into array of strings.
        // one argument per string
        args[0] = strtok(line, " \t\n");

        int i = 0;

        while (i <= 100 && args[i] != NULL) {
            args[++i] = strtok(NULL, " \t\n");
        }

        // if user input is empty
        if (args[0] == NULL) {
            continue;
        }

        // check aliasing
        if (strcmp(args[0], "alias") == 0) {
            if (i < 3) {
                if (i == 1) {  // print
                    printList(head->next);
                    continue;
                } else {  // print single alias
                    Alias_Node *print = checkAlias(head->next, args[1]);
                    if (print != NULL) {
                        char path[512] = "";
                        for (int i = 0; i < print->num; i++) {
                            if (i == print->num - 1) {
                                strcat(path, print->path[i]);
                            } else {
                                strcat(path, print->path[i]);
                                strcat(path, " ");
                            }
                        }
                        fprintf(stdout, "%s %s\n", print->alias, path);
                        fflush(stdout);
                    }
                    continue;
                }
            }

            // check if the alias name is alias, unalias, or exit
            if (strcmp(args[1], "alias") == 0
                    || strcmp(args[1], "unalias") == 0
                    || strcmp(args[1], "exit") == 0) {
                write(STDERR_FILENO, ALIAS_DANGER_ERR,
                        strlen(ALIAS_DANGER_ERR));
                continue;
            }

            // copy user line arguments for path into a path array
            char *path[MAX_LINE_CMDS];
            for (int j = 2; j <= i; j++) {
                if (j <= i) {
                    path[j - 2] = args[j];
                } else {
                    path[j - 2] = NULL;
                }
            }

            // check if alias node exists alread.
            // if so, replace its current path
            Alias_Node *exists = checkAlias(head->next, args[1]);
            if (exists != NULL) {
                addNodePath(exists, path, i - 2);
                exists->num = i - 2;
                continue;
            }

            // if not, add new node
            Alias_Node *new_node = malloc(sizeof(Alias_Node));
            new_node->alias = args[1];
            new_node->next = NULL;
            addNodePath(new_node, path, i - 2);
            new_node->prev = tail;
            new_node->num = i - 2;
            tail->next = new_node;
            tail = new_node;
            continue;
        }

        // check unaliasing
        if (strcmp(args[0], "unalias") == 0) {
            if (i > 2 || i < 2) {
                write(STDERR_FILENO, UNALIAS_PARTS_ERR,
                        strlen(UNALIAS_PARTS_ERR));
                continue;
            } else {  // delete alias specified
                Alias_Node *del = checkAlias(head->next, args[1]);
                if (del == NULL) {
                    continue;
                } else {
                    if (del->next == NULL) {
                        tail = tail->prev;
                        tail->next = NULL;
                        free(del);
                        del = NULL;
                        continue;
                    } else {
                        del->prev->next = del->next;
                        del->next->prev = del->prev;
                        free(del);
                        del = NULL;
                        continue;
                    }
                }
            }
        }

        // check redirect
        if (strstr(redir, ">") != NULL) {
            redir_flag = 1;
        }

        char *forward = strchr(redir, '>');
        char *backward = strrchr(redir, '>');
        char *redir_args[MAX_LINE_CMDS];
        char *redir_output[MAX_LINE_CMDS];

        // check for multiple >
        if (forward != backward && redir_flag) {
            write(STDERR_FILENO, INVALID_REDIRECT,
                    strlen(INVALID_REDIRECT));
            free(redir);
            continue;
        }

        // break up command line into arguments and output
        int redir_count = 0;
        char *before_redirect = strtok(redir, ">");
        char *after_redirect = strtok(NULL, ">");

        redir_output[0] = strtok(after_redirect, " \t\n");
        while (redir_count <= 100 && redir_output[redir_count] != NULL) {
            redir_output[++redir_count] = strtok(NULL, " \t\n");
        }

        // make sure there is only one output
        if (redir_count != 1 && redir_flag) {
            write(STDERR_FILENO, INVALID_REDIRECT,
                    strlen(INVALID_REDIRECT));
            free(redir);
            continue;
        }

        int rd_cmd = 0;
        redir_args[0] = strtok(before_redirect, " \t\n");
        while (rd_cmd <= 100 && redir_args[rd_cmd] != NULL) {
            redir_args[++rd_cmd] = strtok(NULL, " \t\n");
        }

        // make sure there is at least one argument
        if (rd_cmd == 0 && redir_flag) {
            write(STDERR_FILENO, INVALID_REDIRECT,
                    strlen(INVALID_REDIRECT));
            free(redir);
            continue;
        }


        // fork
        pid_t pid = fork();

        if (pid != -1) {
            // child
            if (pid == 0) {
                Alias_Node *alias = checkAlias(head->next, args[0]);
                if (alias != NULL) {
                    execv(alias->path[0], alias->path);
                    fprintf(stderr, "%s: Command not found.\n",
                            alias->path[0]);
                    fflush(stderr);
                    _exit(1);
                }

                if (redir_flag) {
                    int fd = open(redir_output[0], O_WRONLY | O_CREAT
                            | O_TRUNC, 0600);
                    dup2(fd, STDOUT_FILENO);
                    close(fd);


                    execv(redir_args[0], redir_args);
                    fprintf(stderr, "%s: Command not found.\n",
                            redir_args[0]);
                    fflush(stderr);
                    _exit(1);
                }

                execv(args[0], args);

                // if child reached here, exec failed
                fprintf(stderr, "%s: Command not found.\n", args[0]);
                fflush(stderr);
                _exit(1);
            } else {  // parent
                int status;
                waitpid(pid, &status, 0);
            }
        } else {  // fork failed
            printf("mysh: fork failed\n");
        }
        free(line);
        free(redir);
    }
    fclose(input);
}

int main(int argc, char *argv[]) {
    // num command line arguements incorrect
    if (argc > 2) {
        write(STDERR_FILENO, INVALID_CMD_LINE, strlen(INVALID_CMD_LINE));
        exit(1);
    } else if (argc == 1) {  // interactive mode
        interactive_mode();
    } else {  // batch mode
        batch_mode(argv);
    }
}
