/* Q2. Two-command pipeline: command1 | command2.
   Compile: gcc -Wall -Wextra -o two_pipe 02_two_command_pipe.c
   Run: ./two_pipe "cat file.txt" "wc -l" */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

static int parse(char *s, char *a[], int cap) {
    int n=0; char *save=NULL, *t=strtok_r(s," \t",&save);
    while(t && n<cap-1){a[n++]=t;t=strtok_r(NULL," \t",&save);} a[n]=NULL; return n;
}
int main(int argc,char **argv) {
    if(argc!=3){fprintf(stderr,"Usage: %s 'cmd1 args' 'cmd2 args'\n",argv[0]);return 2;}
    char c1[512],c2[512]; snprintf(c1,sizeof c1,"%s",argv[1]);snprintf(c2,sizeof c2,"%s",argv[2]);
    char *a1[32],*a2[32]; if(!parse(c1,a1,32)||!parse(c2,a2,32))return 2;
    int pfd[2]; if(pipe(pfd)<0){perror("pipe");return 1;}
    pid_t p1=fork(); if(p1<0){perror("fork");return 1;}
    if(p1==0){dup2(pfd[1],STDOUT_FILENO);close(pfd[0]);close(pfd[1]);execvp(a1[0],a1);perror(a1[0]);_exit(127);}
    pid_t p2=fork(); if(p2<0){perror("fork");close(pfd[0]);close(pfd[1]);waitpid(p1,NULL,0);return 1;}
    if(p2==0){dup2(pfd[0],STDIN_FILENO);close(pfd[0]);close(pfd[1]);execvp(a2[0],a2);perror(a2[0]);_exit(127);}
    close(pfd[0]);close(pfd[1]);waitpid(p1,NULL,0);waitpid(p2,NULL,0);return 0;
}
