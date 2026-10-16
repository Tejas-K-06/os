/*
 * OS Laboratory Test-1, Question 1
 * File: 612415074-shell_1.c
 *
 * A small shell that executes commands using execv().
 * The executable path must be supplied as the first argument.
 * The special command "last" prints the exit status of the last child.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS  64

int main(void)
{
    char input[MAX_INPUT];
    int last_status = -1;
    int have_last_status = 0;

    for (;;) {
        char *args[MAX_ARGS];
        int argc = 0;
        char *token;
        pid_t pid;
        int status;

        printf("myshell$ ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        /* Ignore blank lines. */
        if (input[0] == '\0')
            continue;

        /* Print the status of the most recently executed child. */
        if (strcmp(input, "last") == 0) {
            if (have_last_status)
                printf("Last process status: %d\n", last_status);
            else
                printf("No process has been executed yet.\n");
            continue;
        }

        /* Split the input into an executable path and its arguments. */
        token = strtok(input, " \t");
        while (token != NULL && argc < MAX_ARGS - 1) {
            args[argc++] = token;
            token = strtok(NULL, " \t");
        }
        args[argc] = NULL;

        if (argc == 0)
            continue;

        pid = fork();

        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            /* args[0] is the full path supplied by the user. */
            execv(args[0], args);

            /* execv returns only when execution fails. */
            perror("execv");
            _exit(127);
        }

        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid");
            last_status = -1;
            have_last_status = 1;
            continue;
        }

        if (WIFEXITED(status)) {
            last_status = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            /* Use a negative value to distinguish signal termination. */
            last_status = -WTERMSIG(status);
        } else {
            last_status = -1;
        }

        have_last_status = 1;
        printf("Child process status: %d\n", last_status);
    }

    return 0;
}
