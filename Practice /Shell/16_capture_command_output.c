/* Q16. Parent captures a command's stdout using a pipe.
   Run: ./capture_output ls -l */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc,char**argv){if(argc<2){fprintf(stderr,"Usage: %s command [args...]\n",argv[0]);return 2;}int fd[2];if(pipe(fd)<0){perror("pipe");return 1;}
 pid_t p=fork();if(p<0){perror("fork");return 1;}if(p==0){close(fd[0]);if(dup2(fd[1],STDOUT_FILENO)<0)_exit(126);close(fd[1]);execvp(argv[1],&argv[1]);perror(argv[1]);_exit(127);}
 close(fd[1]);char b[256];ssize_t n;while((n=read(fd[0],b,sizeof b))>0)if(write(STDOUT_FILENO,b,(size_t)n)!=n){perror("write");break;}close(fd[0]);waitpid(p,NULL,0);return n<0?1:0;}
