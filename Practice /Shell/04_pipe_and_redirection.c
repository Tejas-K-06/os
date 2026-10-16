/* Q4. Equivalent to: cat input.txt | grep pattern > output.txt
   Compile: gcc -Wall -Wextra -o pipe_redirect 04_pipe_and_redirection.c
   Run: ./pipe_redirect input.txt pattern output.txt */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc,char **argv){
    if(argc!=4){fprintf(stderr,"Usage: %s input pattern output\n",argv[0]);return 2;}
    int pfd[2];if(pipe(pfd)<0){perror("pipe");return 1;}
    pid_t a=fork();if(a<0){perror("fork");return 1;}
    if(a==0){int in=open(argv[1],O_RDONLY);if(in<0){perror(argv[1]);_exit(1);}dup2(in,0);dup2(pfd[1],1);close(in);close(pfd[0]);close(pfd[1]);char *av[]={"cat",NULL};execvp(av[0],av);perror("cat");_exit(127);}
    pid_t b=fork();if(b<0){perror("fork");close(pfd[0]);close(pfd[1]);waitpid(a,NULL,0);return 1;}
    if(b==0){int out=open(argv[3],O_WRONLY|O_CREAT|O_TRUNC,0644);if(out<0){perror(argv[3]);_exit(1);}dup2(pfd[0],0);dup2(out,1);close(out);close(pfd[0]);close(pfd[1]);char *av[]={"grep",argv[2],NULL};execvp(av[0],av);perror("grep");_exit(127);}
    close(pfd[0]);close(pfd[1]);waitpid(a,NULL,0);waitpid(b,NULL,0);return 0;
}
