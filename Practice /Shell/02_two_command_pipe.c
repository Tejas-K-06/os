/* Q2: Run command1 | command2. Arguments are passed as quoted command strings.
 * Parser intentionally supports simple whitespace-separated arguments only.
 * Build: gcc -std=c11 -Wall -Wextra -pedantic -o two_pipe 02_two_command_pipe.c
 * Run: ./two_pipe "cat file.txt" "wc -l"
 */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
static int parse(char *s, char *a[], int cap) {
    int n=0; char *save=NULL;
    for(char *t=strtok_r(s," \t",&save); t && n<cap-1; t=strtok_r(NULL," \t",&save)) a[n++]=t;
    a[n]=NULL; return n;
}
static int wait_status(pid_t p) {
    int st; while(waitpid(p,&st,0)<0) { if(errno==EINTR) continue; perror("waitpid"); return 1; }
    return WIFEXITED(st) ? WEXITSTATUS(st) : (WIFSIGNALED(st) ? 128+WTERMSIG(st) : 1);
}
int main(int argc,char **argv) {
    if(argc!=3){fprintf(stderr,"Usage: %s 'cmd1 args' 'cmd2 args'\n",argv[0]);return 2;}
    char c1[512],c2[512]; if(strlen(argv[1])>=sizeof c1 || strlen(argv[2])>=sizeof c2){fprintf(stderr,"Command too long.\n");return 2;}
    strcpy(c1,argv[1]); strcpy(c2,argv[2]);
    char *a1[32],*a2[32]; if(!parse(c1,a1,32)||!parse(c2,a2,32)){fprintf(stderr,"Commands cannot be empty.\n");return 2;}
    int fd[2]; if(pipe(fd)<0){perror("pipe");return 1;}
    pid_t p1=fork(); if(p1<0){perror("fork");close(fd[0]);close(fd[1]);return 1;}
    if(p1==0){if(dup2(fd[1],STDOUT_FILENO)<0){perror("dup2");_exit(126);}close(fd[0]);close(fd[1]);execvp(a1[0],a1);perror(a1[0]);_exit(errno==ENOENT?127:126);}
    pid_t p2=fork(); if(p2<0){perror("fork");close(fd[0]);close(fd[1]);wait_status(p1);return 1;}
    if(p2==0){if(dup2(fd[0],STDIN_FILENO)<0){perror("dup2");_exit(126);}close(fd[0]);close(fd[1]);execvp(a2[0],a2);perror(a2[0]);_exit(errno==ENOENT?127:126);}
    close(fd[0]);close(fd[1]); int s1=wait_status(p1),s2=wait_status(p2); return s2 ? s2 : s1;
}
