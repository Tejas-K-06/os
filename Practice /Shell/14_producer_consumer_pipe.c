/* Q14. Producer child writes integers; parent consumes them from a pipe. */
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void){int fd[2];if(pipe(fd)<0){perror("pipe");return 1;}pid_t p=fork();if(p<0){perror("fork");return 1;}
 if(p==0){close(fd[0]);for(int i=1;i<=5;i++)if(write(fd[1],&i,sizeof i)!=(ssize_t)sizeof i)_exit(1);close(fd[1]);_exit(0);}
 close(fd[1]);int x;ssize_t n;while((n=read(fd[0],&x,sizeof x))>0){if(n==(ssize_t)sizeof x)printf("Consumed: %d\n",x);}close(fd[0]);waitpid(p,NULL,0);return 0;}
