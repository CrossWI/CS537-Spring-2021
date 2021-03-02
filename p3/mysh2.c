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
static char *INVALID_LINE_SIZE = "Error: Line exceeds 512 characters.\n";
static char *INVALID_REDIRECT = "Redirection misformatted.\n";
static char *FILE_OPEN_ERR = "Error: File could not be opened.\n";
static char *UNALIAS_PARTS_ERR = "unalias: Incorrect number of arguments.\n";
static char *ALIAS_DANGER_ERR = "alias: Too dangerous to alias that.\n";

// command prompt
static char *COMMAND_PROMPT = "mysh> ";

typedef struct Alias_Node {
    char *alias;
    char *path;
    struct Alias_Node* next;
} Alias_Node;

Alias_Node *head = NULL;
Alias_Node *tail = NULL;

void printList () {
  if (head == NULL) {
    return;
  }

  Alias_Node *temp = head;

  while (temp != NULL) {
    printf("%s %s\n", temp->alias, temp->path);
    temp = temp->next;
  }
  return;
}

void appendNode(char* alias, char* path) {
    Alias_Node *new_node = malloc(sizeof(Alias_Node));
 
    new_node->alias = strdup(alias);
    new_node->path = strdup(path);
    new_node->next = NULL;

    if (head == NULL) {
        head = new_node;
        return;
    }

    if (strcmp(head->alias, alias) == 0) {
      strcpy(head->path, path);
      return;
    }

    Alias_Node *temp = head;

    while (temp->next != NULL) {
      temp = temp->next;
      if (strcmp(temp->alias, alias) == 0) {
        strcpy(temp->path, path);
        return;
      }
    }
    temp->next = new_node;
    return;
}

void deleteNode(char* alias) {
  Alias_Node *temp = head;
  Alias_Node *prev = temp;

  while (temp->next != NULL) {
    if(strcmp(temp->alias, alias) == 0) {
      prev->next = temp->next;
      free(temp);
      return;
    }
    prev = temp;
    temp = temp->next;
  }
  return;
}

Alias_Node *checkAlias(char* alias) {
  Alias_Node *temp = head;

  while (temp->next != NULL) {
    if(strcmp(temp->alias, alias) == 0) {
      return temp;
    }
    temp = temp->next;
  }
  return NULL;
}

char** copyArray(char* src[], char* dest[]) {
  int i = 0;
  dest[0] = src[0];
  while (i <= 100 && src[i] != NULL) {
    i++;
    dest[i] = src[i];
  }
}

void interactive_mode() {
  while (1) {
    write(STDOUT_FILENO, COMMAND_PROMPT, strlen(COMMAND_PROMPT));
    char userline[MAX_CMD_LINE_SIZE];

    // wait for user input
    if (fgets(userline, MAX_CMD_LINE_SIZE, stdin) == 0) {
      write(STDOUT_FILENO, "\n", 2);
      return;
    } else {
      // execute
      if (strcmp(userline, "exit\n") == 0) {
        return;
      }

      char *line = strdup(userline);
      char *args[MAX_LINE_CMDS];
      int redir_flag = 0;

      args[0] = strtok(line, " \t\n");

      int i = 0;

      while (i <= 100 && args[i] != NULL) {
        args[++i] = strtok(NULL, " \t\n");
      }

      if (args[0] == NULL) {
        continue;
      }

      if (strcmp(args[0], "alias") == 0) {
        if (strcmp(args[1], "alias") == 0 || strcmp(args[1], "unalias") == 0 || strcmp(args[1], "exit") == 0) {
          write(STDERR_FILENO, ALIAS_DANGER_ERR, strlen(ALIAS_DANGER_ERR));
          continue;
        } else if (args[1] == NULL) {
          printList();
          continue;
        } else {
          char *alias_arg = strdup(userline);
          char *temp = strtok(alias_arg, args[1]);
          char *path = strtok(NULL, " \t\n");
          appendNode(args[1], path);
          free(alias_arg);
          continue;
        }
      }

      if (strcmp(args[0], "unalias") == 0) {
        if (i > 2 || i < 2) {
          write(STDERR_FILENO, UNALIAS_PARTS_ERR, strlen(UNALIAS_PARTS_ERR));
          continue;
        } else {
          deleteNode(args[1]);
          continue;
        }
      }

      // check redirect
      if (strstr(line, ">") != NULL) {
        redir_flag = 1;
  
        char *forward = strchr(redir, '>');
        char *backward = strrchr(redir, '>');
        char *redir_args[MAX_LINE_CMDS];
        char *redir_output[MAX_LINE_CMDS];
        char *redir = strdup(userline);

        if (forward != backward) {
          write(STDERR_FILENO, INVALID_REDIRECT, strlen(INVALID_REDIRECT));
          free(redir);
          continue;
        }

        int redir_count = 0;
    	  char *before_redirect = strtok(redir, ">");
    		char *after_redirect = strtok(NULL, ">");

        redir_output[0] = strtok(after_redirect, " \t\n");
        while (redir_count <= 100 && redir_output[redir_count] != NULL) {
          redir_output[++redir_count] = strtok(NULL, " \t\n");
        }

        if (redir_count != 1) {
          write(STDERR_FILENO, INVALID_REDIRECT, strlen(INVALID_REDIRECT));
          free(redir);
          continue;
        }

        int rd_cmd = 0;
        redir_args[0] = strtok(before_redirect, " \t\n");
        while (i <= 100 && redir_args[rd_cmd] != NULL) {
          redir_args[++rd_cmd] = strtok(NULL, " \t\n");
        }

        if (rd_cmd == 0) {
          write(STDERR_FILENO, INVALID_REDIRECT, strlen(INVALID_REDIRECT));
          free(redir);
          continue;
        }
        args = copyArray(redir_args, args);
        free(redir);

        int fd = open(redir_output[0], O_WRONLY | O_CREAT | O_TRUNC, 0600);
        dup2(fd, STDOUT_FILENO);
        close(fd);
      }

      // check linked list for alias
      Alias_Node *alias_flag = checkAlias(args[0]);
      if (alias_flag) {
        char *path_args[MAX_LINE_CMDS];
        char *path_str = strdup(node->path);

        int index = 0;
        path_args[0] = strtok(path_str, " \t\n");

        while (index <= 100 && path_args[index] != NULL) {
          path_args[++index] = strtok(NULL, " \t\n");
        }
        free(path_str);
        args = copyArray(path_args, args);
      }

      pid_t pid = fork();
      
      if (pid != -1) {
        // child
        if (pid == 0) {
          execv(args[0], args);

          // if child reached here, exec failed
          fprintf(stderr, "%s: Command not found.\n", args[0]);
          fflush(stderr);
          _exit(1);
        } else { // parent
          int status;
          waitpid(pid, &status, 0);
        }
      } else { // fork failed
        printf("mysh: fork failed\n");
      }
      free(line);
      free(redir);
    }
  }
}

void batch_mode () {
  
}

int main(int argc, char *argv[]) {
  // num command line arguements incorrect
  if (argc > 2) {
    write(STDERR_FILENO, INVALID_CMD_LINE, strlen(INVALID_CMD_LINE));
    exit(1);
  }

  // interactive mode
  else if (argc == 1) {
    interactive_mode();
  }

  // batch mode
  else {
    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
      fprintf(stderr, "Error: Cannot open file %s.\n", argv[1]);
      fflush(stderr);
      exit(1);
    }

    // creat a buffer array of line size
    char buf[MAX_CMD_LINE_SIZE];

    // read file line by line
    while (fgets(buf, MAX_CMD_LINE_SIZE, input) != NULL) {
      if (strlen(buf) > MAX_CMD_LINE_SIZE) {
        write(STDERR_FILENO, INVALID_LINE_SIZE, strlen(INVALID_LINE_SIZE));
      }
      write(STDOUT_FILENO, buf, strlen(buf));
    }
    fclose(input);
  }
}