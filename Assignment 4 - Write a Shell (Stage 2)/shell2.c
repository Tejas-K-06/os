#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 1024
#define MAX_ARGS 100

char custom_path[MAX_LINE] = "/usr/bin:/bin";

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

void execute_with_execl(char *full_path, char **args) {
    int count = 0;
    while (args[count] != NULL) {
        count++;
    }

    switch (count) {
        case 1: execl(full_path, args[0], NULL); break;
        case 2: execl(full_path, args[0], args[1], NULL); break;
        case 3: execl(full_path, args[0], args[1], args[2], NULL); break;
        case 4: execl(full_path, args[0], args[1], args[2], args[3], NULL); break;
        case 5: execl(full_path, args[0], args[1], args[2], args[3], args[4], NULL); break;
        case 6: execl(full_path, args[0], args[1], args[2], args[3], args[4], args[5], NULL); break;
        default:
            printf("Error: Too many arguments for this shell.\n");
            exit(EXIT_FAILURE);
    }
}

void custom_exec(char **args) {
    if (args == NULL || args[0] == NULL)
    exit(EXIT_FAILURE);

    if (strchr(args[0], '/') != NULL) {
        execute_with_execl(args[0], args);
        perror(args[0]);
        exit(EXIT_FAILURE);
    }

    char path_copy[MAX_LINE];
    strcpy(path_copy, custom_path);

    char *dir = strtok(path_copy, ":");
    char full_path[MAX_LINE];

    while (dir != NULL) {
        snprintf(full_path, MAX_LINE, "%s/%s", dir, args[0]);
        execute_with_execl(full_path, args);
        dir = strtok(NULL, ":");
    }

    printf("%s: command not found\n", args[0]);
    exit(EXIT_FAILURE);
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
    if (strncmp(args[0], "PATH=", 5) == 0) {
        strcpy(custom_path, args[0] + 5);
        printf("PATH updated successfully to: %s\n", custom_path);
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
                int i = 0;
                while (args[i] != NULL) {
                    
                    if (strcmp(args[i], ">") == 0) {
                        args[i] = NULL; 
                        
                        if (args[i+1] == NULL) {
                            printf("Error: missing filename for output redirection\n");
                            exit(EXIT_FAILURE);
                        }
                        
                        int fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd < 0) {
                            perror(args[i+1]);
                            exit(EXIT_FAILURE);
                        }
                        
                        close(1);
                        dup(fd);
                        close(fd);
                    }
                    
                    else if (strcmp(args[i], "<") == 0) {
                        args[i] = NULL; 
                        
                        if (args[i+1] == NULL) {
                            printf("Error: missing filename for input redirection\n");
                            exit(EXIT_FAILURE);
                        }
                        
                        int fd = open(args[i+1], O_RDONLY);
                        if (fd < 0) {
                            perror(args[i+1]);
                            exit(EXIT_FAILURE);
                        }
                        
                        close(0);
                        dup(fd);
                        close(fd);
                    }
                    i++;
                }
                custom_exec(args);
            }
            else {
                wait(NULL);
            }
        }
    }
    return 0;
}
