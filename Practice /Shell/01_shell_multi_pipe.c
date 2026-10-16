/* Q1. Shell supporting pipelines of arbitrary length (simple whitespace parser).
   Example: ./01_shell_multi_pipe then enter: ls -l | grep .c | wc -l
   Compile: gcc -Wall -Wextra -o shell 01_shell_multi_pipe.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define LINE 1024
#define CMDS 16
#define ARGS 32

static int split(char *s, char *v[], int max, char delim) {
    int n = 0;
    char d[2] = { delim, '\0' };
    char *save = NULL, *t = strtok_r(s, d, &save);
    while (t && n < max) { while (*t == ' ' || *t == '\t') t++; v[n++] = t; t = strtok_r(NULL, d, &save); }
    return n;
}
int main(void) {
    char line[LINE];
    while (printf("myshell> "), fflush(stdout), fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "exit")) break;
        char *parts[CMDS]; int n = split(line, parts, CMDS, '|');
        if (n == 0) continue;
        int pipes[CMDS-1][2], made = 0, failed = 0;
        for (int i=0; i<n-1; i++) { if (pipe(pipes[i]) < 0) { perror("pipe"); failed=1; break; } made++; }
        if (failed) { for(int i=0;i<made;i++){close(pipes[i][0]);close(pipes[i][1]);} continue; }
        pid_t pids[CMDS]; int started=0;
        for (int i=0; i<n; i++) {
            char *argv[ARGS]; int ac=split(parts[i], argv, ARGS-1, ' '); argv[ac]=NULL;
            if (!ac) { fprintf(stderr,"empty pipeline command\n"); failed=1; break; }
            pid_t p=fork();
            if (p<0) { perror("fork"); failed=1; break; }
            if (p==0) {
                if(i>0 && dup2(pipes[i-1][0], STDIN_FILENO)<0) _exit(126);
                if(i<n-1 && dup2(pipes[i][1], STDOUT_FILENO)<0) _exit(126);
                for(int j=0;j<n-1;j++){close(pipes[j][0]);close(pipes[j][1]);}
                execvp(argv[0], argv); perror(argv[0]); _exit(127);
            }
            pids[started++]=p;
        }
        for(int i=0;i<n-1;i++){close(pipes[i][0]);close(pipes[i][1]);}
        for(int i=0;i<started;i++) waitpid(pids[i],NULL,0);
        (void)failed;
    }
    return 0;
}
