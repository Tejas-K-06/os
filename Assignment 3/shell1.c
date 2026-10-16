#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 100

int read_user_input(char *buffer) {
    printf("myshell> ");
    return fgets(buffer, MAX_LINE, stdin) != NULL;
}

void parse_input(char *buffer, char **args) {
    int count = 0;
    char *token = strtok(buffer, " \n\t");
    while (token != NULL && count < MAX_ARGS - 1) {
        args[count] = token;
        count++;
        token = strtok(NULL, " \n\t");
    }
    args[count] = NULL; 
}

int execute_builtin(char **args) {
    if (args[0] == NULL) {
        return 1; 
    }
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            printf("cd: missing argument\n");
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd failed");
            }
        }
        return 1;
    }
    return 0; 
}

int main() {
    char buffer[MAX_LINE];
    char *args[MAX_ARGS];
    while (1) {
        int has_input = read_user_input(buffer);
        if (has_input == 0) {
            printf("\n");
            break;
        }
        parse_input(buffer, args);
        if (args[0] == NULL) {
            continue;
        }
        if (!execute_builtin(args)) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork failed");
            }
            else if (pid == 0) {
                execvp(args[0], args);
                perror("Command not found");
                exit(1);
            }
            else {
                wait(NULL);
            }
        }
    }
    return 0;
}