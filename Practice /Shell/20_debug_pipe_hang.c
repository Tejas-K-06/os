/* Q20. Corrected pipeline demonstrating the essential close() calls.
   If the parent keeps the write end open, wc may never see EOF and hangs. */
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void){int fd[2];if(pipe(fd)<0){perror("pipe");return 1;}pid_t a=fork();if(a<0){perror("fork");return 1;}
 if(a==0){if(dup2(fd[1],STDOUT_FILENO)<0)_exit(126);close(fd[0]);close(fd[1]);char*av[]={"printf","hello\\n",NULL};execvp(av[0],av);perror("printf");_exit(127);}
 pid_t b=fork();if(b<0){perror("fork");close(fd[0]);close(fd[1]);waitpid(a,NULL,0);return 1;}
 if(b==0){if(dup2(fd[0],STDIN_FILENO)<0)_exit(126);close(fd[0]);close(fd[1]);char*av[]={"wc","-c",NULL};execvp(av[0],av);perror("wc");_exit(127);}
 close(fd[0]);close(fd[1]);waitpid(a,NULL,0);waitpid(b,NULL,0);return 0;}
