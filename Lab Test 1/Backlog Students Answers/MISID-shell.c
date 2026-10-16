/*
 * Operating Systems Lab
 * Q1: Shell using execl() only
 * Filename: MISID-shell.c
 *
 * Supports up to three arguments after the executable name.
 * The executable should be entered using its path, e.g. /bin/ls -l.
 * Type "history" to display commands entered in this shell.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 512
#define MAX_ARGS 4       /* executable + at most 3 user arguments */
#define HISTORY_SIZE 100

static char history[HISTORY_SIZE][MAX_LINE];
static int history_count = 0;

static void add_history(const char *line)
{
    int index;

    if (history_count < HISTORY_SIZE) {
        index = history_count++;
    } else {
        /* Keep the most recent HISTORY_SIZE commands. */
        memmove(history, history + 1,
                (HISTORY_SIZE - 1) * sizeof(history[0]));
        index = HISTORY_SIZE - 1;
    }

    snprintf(history[index], MAX_LINE, "%s", line);
}

static void show_history(void)
{
    int i;

    for (i = 0; i < history_count; i++)
        printf("%d  %s\n", i + 1, history[i]);
}

int main(void)
{
    char line[MAX_LINE];

    while (1) {
        char *args[MAX_ARGS + 1];
        char *token;
        int argc = 0;
        pid_t pid;
        int status;

        printf("myshell$ ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL)
            break;

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0')
            continue;

        /* Record every non-empty command, including "history" and "exit". */
        add_history(line);

        if (strcmp(line, "exit") == 0)
            break;

        if (strcmp(line, "history") == 0) {
            show_history();
            continue;
        }

        token = strtok(line, " \t");
        while (token != NULL && argc < MAX_ARGS) {
            args[argc++] = token;
            token = strtok(NULL, " \t");
        }
        args[argc] = NULL;

        if (token != NULL) {
            fprintf(stderr, "Error: maximum 3 arguments are allowed.\n");
            continue;
        }

        if (argc == 0)
            continue;

        pid = fork();

        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            /*
             * execl() takes a variable list of arguments, not an argv array.
             * Handle 0, 1, 2, or 3 arguments explicitly.
             */
            switch (argc) {
            case 1:
                execl(args[0], args[0], (char *)NULL);
                break;
            case 2:
                execl(args[0], args[0], args[1], (char *)NULL);
                break;
            case 3:
                execl(args[0], args[0], args[1], args[2], (char *)NULL);
                break;
            case 4:
                execl(args[0], args[0], args[1], args[2], args[3],
                      (char *)NULL);
                break;
            }

            perror("execl");
            _exit(127);
        }

        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid");
            continue;
        }

        if (WIFEXITED(status))
            printf("Child exit status: %d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("Child terminated by signal: %d\n", WTERMSIG(status));
    }

    return 0;
}
