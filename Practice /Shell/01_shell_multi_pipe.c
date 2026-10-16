/* Q1: Interactive shell supporting pipelines of up to 16 commands.
 * Supports simple whitespace-separated arguments; quotes and escapes are not implemented.
 * Build: gcc -std=c11 -Wall -Wextra -pedantic -o shell 01_shell_multi_pipe.c
 */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define LINE 1024
#define MAX_CMDS 16
#define MAX_ARGS 32

static int split(char *s, char *v[], int cap, char delim) {
    int n = 0;
    char ds[2] = {delim, '\0'};
    char *save = NULL;
    for (char *t = strtok_r(s, ds, &save); t && n < cap; t = strtok_r(NULL, ds, &save)) {
        while (*t == ' ' || *t == '\t') t++;
        if (*t) v[n++] = t;
    }
    return n;
}

static int wait_all(pid_t pids[], int n) {
    int result = 0;
    for (int i = 0; i < n; i++) {
        int st;
        while (waitpid(pids[i], &st, 0) < 0) {
            if (errno == EINTR) continue;
            perror("waitpid"); result = 1; break;
        }
        if (WIFEXITED(st) && WEXITSTATUS(st) != 0) result = WEXITSTATUS(st);
        else if (WIFSIGNALED(st)) result = 128 + WTERMSIG(st);
    }
    return result;
}

int main(void) {
    char line[LINE];
    while (fputs("myshell> ", stdout), fflush(stdout), fgets(line, sizeof line, stdin)) {
        if (!strchr(line, '\n') && !feof(stdin)) {
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            fprintf(stderr, "Input too long (maximum %d characters).\n", LINE - 2);
            continue;
        }
        line[strcspn(line, "\n")] = '\0';
        if (!strcmp(line, "exit")) break;
        char *parts[MAX_CMDS];
        int n = split(line, parts, MAX_CMDS, '|');
        if (!n) continue;
        if (strchr(line, '|') && n == 1 && !*parts[0]) continue;

        char *argvs[MAX_CMDS][MAX_ARGS];
        for (int i = 0; i < n; i++) {
            int ac = split(parts[i], argvs[i], MAX_ARGS - 1, ' ');
            argvs[i][ac] = NULL;
            if (!ac) { fprintf(stderr, "Empty command in pipeline.\n"); n = 0; break; }
        }
        if (!n) continue;

        int pipes[MAX_CMDS - 1][2], made = 0, failed = 0;
        for (int i = 0; i < n - 1; i++) {
            if (pipe(pipes[i]) < 0) { perror("pipe"); failed = 1; break; }
            made++;
        }
        if (failed) { for (int i = 0; i < made; i++) { close(pipes[i][0]); close(pipes[i][1]); } continue; }

        pid_t pids[MAX_CMDS]; int started = 0;
        for (int i = 0; i < n; i++) {
            pid_t p = fork();
            if (p < 0) { perror("fork"); failed = 1; break; }
            if (p == 0) {
                if (i > 0 && dup2(pipes[i-1][0], STDIN_FILENO) < 0) { perror("dup2"); _exit(126); }
                if (i < n-1 && dup2(pipes[i][1], STDOUT_FILENO) < 0) { perror("dup2"); _exit(126); }
                for (int j = 0; j < made; j++) { close(pipes[j][0]); close(pipes[j][1]); }
                execvp(argvs[i][0], argvs[i]); perror(argvs[i][0]); _exit(errno == ENOENT ? 127 : 126);
            }
            pids[started++] = p;
        }
        for (int i = 0; i < made; i++) { close(pipes[i][0]); close(pipes[i][1]); }
        int status = wait_all(pids, started);
        if (failed) fprintf(stderr, "Pipeline could not be started completely.\n");
        else if (status) fprintf(stderr, "Pipeline exit status: %d\n", status);
    }
    return ferror(stdin) ? 1 : 0;
}
