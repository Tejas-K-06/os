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

void parse_pipes(char *buffer, char **commands, int *num_commands) {
    int count = 0;
    char *token = strtok(buffer, "|");
    while (token != NULL && count < MAX_ARGS - 1) {
        commands[count] = token;
        count++;
        token = strtok(NULL, "|");
    }
    commands[count] = NULL;
    *num_commands = count;
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
    char *commands[MAX_ARGS];
    char *args[MAX_ARGS];
    int num_commands = 0;

    while (1) {
        int has_input = read_user_input(buffer);
        if (has_input == 0) {
            printf("\n");
            break;
        }

        int is_empty = 1;
        for (int i = 0; buffer[i] != '\0' && buffer[i] != '\n'; i++) {
            if (buffer[i] != ' ' && buffer[i] != '\t') {
                is_empty = 0;
                break;
            }
        }
        if (is_empty) {
            continue;
        }

        parse_pipes(buffer, commands, &num_commands);    //to count and store the unique commands 

        if (num_commands == 0) continue;

        if (num_commands == 1) {           //to handle built in commands which do not require pipes
            char temp_cmd[MAX_LINE];
            strcpy(temp_cmd, commands[0]);
            parse_input(temp_cmd, args);
            if (args[0] != NULL && execute_builtin(args)) {
                continue;
            }
        }

        int pipes[MAX_ARGS][2];
        for (int i = 0; i < num_commands - 1; i++) {  //we have to create n - 1 pipes for n commands using the pipe sys call
            if (pipe(pipes[i]) < 0) {
                perror("pipe has failed");
            }
        }

        for (int i = 0; i < num_commands; i++) {
            parse_input(commands[i], args);
            if (args[0] == NULL) continue;

            int pid = fork(); //fork a new child process for this particular command
            if (pid < 0) {
                perror("fork has failed");
            } else if (pid == 0) {
                if (i > 0) {
                    dup2(pipes[i-1][0], STDIN_FILENO); //stdin is 0
                }
                if (i < num_commands - 1) {
                    dup2(pipes[i][1], STDOUT_FILENO); //stdout is 1
                }
                for (int j = 0; j < num_commands - 1; j++) {
                    close(pipes[j][0]); //we have to close the read end of the pipe
                    close(pipes[j][1]); //we have to close the write end of the pipe
                }
                
                int j = 0;
                while (args[j] != NULL) {
                    if (strcmp(args[j], ">") == 0) {
                        args[j] = NULL; 
                        
                        if (args[j+1] == NULL) {
                            printf("Error: missing filename for output redirection\n");
                            exit(EXIT_FAILURE); //EXIT_FAILURE is 1 
                        }
                        
                        int fd = open(args[j+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd < 0) {
                            perror(args[j+1]);
                            exit(EXIT_FAILURE);
                        }
                        
                        close(STDOUT_FILENO);
                        dup(fd);
                        close(fd);
                    }
                    else if (strcmp(args[j], "<") == 0) {
                        args[j] = NULL; 
                        
                        if (args[j+1] == NULL) {
                            printf("Error: missing filename for input redirection\n");
                            exit(EXIT_FAILURE);
                        }
                        
                        int fd = open(args[j+1], O_RDONLY);
                        if (fd < 0) {
                            perror(args[j+1]);
                            exit(EXIT_FAILURE);
                        }
                        
                        close(STDIN_FILENO);
                        dup(fd);
                        close(fd);
                    }
                    j++;
                }
                
                if (execute_builtin(args)) {
                    exit(0);
                }
                
                custom_exec(args);
            }
        }
        
        for (int i = 0; i < num_commands - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
        
        for (int i = 0; i < num_commands; i++) {
            wait(NULL);
        }
    }
    return 0;
}
