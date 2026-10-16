/* Q18. Minimal robust one-stage pipeline: command1 | command2.
   Run: ./safe_pipe "printf hello" "wc -c" */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
static int parse(char*s,char*a[],int cap){int n=0;char*sv=NULL,*t=strtok_r(s," \t",&sv);while(t&&n<cap-1){a[n++]=t;t=strtok_r(NULL," \t",&sv);}a[n]=NULL;return n;}
int main(int argc,char**argv){if(argc!=3){fprintf(stderr,"Usage: %s 'cmd1' 'cmd2'\n",argv[0]);return 2;}char b1[512],b2[512];snprintf(b1,sizeof b1,"%s",argv[1]);snprintf(b2,sizeof b2,"%s",argv[2]);char*a1[32],*a2[32];if(!parse(b1,a1,32)||!parse(b2,a2,32))return 2;
 int fd[2];if(pipe(fd)<0){perror("pipe");return 1;}pid_t p1=fork();if(p1<0){perror("fork");close(fd[0]);close(fd[1]);return 1;}
 if(p1==0){if(dup2(fd[1],1)<0){perror("dup2");_exit(126);}close(fd[0]);close(fd[1]);execvp(a1[0],a1);perror(a1[0]);_exit(127);}
 pid_t p2=fork();if(p2<0){perror("fork");close(fd[0]);close(fd[1]);waitpid(p1,NULL,0);return 1;}
 if(p2==0){if(dup2(fd[0],0)<0){perror("dup2");_exit(126);}close(fd[0]);close(fd[1]);execvp(a2[0],a2);perror(a2[0]);_exit(127);}
 close(fd[0]);close(fd[1]);waitpid(p1,NULL,0);waitpid(p2,NULL,0);return 0;}
