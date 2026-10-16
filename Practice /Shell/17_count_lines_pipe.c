/* Q17. Child writes text; parent counts newline characters read from pipe. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void){int fd[2];if(pipe(fd)<0){perror("pipe");return 1;}pid_t p=fork();if(p<0){perror("fork");return 1;}
 if(p==0){close(fd[0]);const char msg[]="first line\nsecond line\nthird line\n";write(fd[1],msg,sizeof msg-1);close(fd[1]);_exit(0);}
 close(fd[1]);char b[128];ssize_t n;int lines=0;while((n=read(fd[0],b,sizeof b))>0)for(ssize_t i=0;i<n;i++)if(b[i]=='\n')lines++;close(fd[0]);waitpid(p,NULL,0);printf("Line count: %d\n",lines);return 0;}
