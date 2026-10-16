/* Q7. Demonstrates grep PATTERN FILE | wc -l using pipe/fork/exec. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc,char**argv){
 if(argc!=3){fprintf(stderr,"Usage: %s pattern file\n",argv[0]);return 2;}
 int fd[2];if(pipe(fd)<0){perror("pipe");return 1;}
 pid_t a=fork();if(a<0){perror("fork");return 1;}
 if(a==0){dup2(fd[1],1);close(fd[0]);close(fd[1]);char *av[]={"grep",argv[1],argv[2],NULL};execvp(av[0],av);perror("grep");_exit(127);}
 pid_t b=fork();if(b<0){perror("fork");return 1;}
 if(b==0){dup2(fd[0],0);close(fd[0]);close(fd[1]);char *av[]={"wc","-l",NULL};execvp(av[0],av);perror("wc");_exit(127);}
 close(fd[0]);close(fd[1]);waitpid(a,NULL,0);waitpid(b,NULL,0);return 0;
}
