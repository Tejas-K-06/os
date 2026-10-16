/* Q3. Run a command with optional < input and > output redirection.
   Compile: gcc -Wall -Wextra -o redirect 03_input_output_redirect.c
   Example: ./redirect sort input.txt output.txt */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc,char **argv) {
    if(argc<2||argc>4){fprintf(stderr,"Usage: %s command [input-file [output-file]]\n",argv[0]);return 2;}
    pid_t p=fork(); if(p<0){perror("fork");return 1;}
    if(p==0){
        if(argc>=3){int fd=open(argv[2],O_RDONLY);if(fd<0){perror(argv[2]);_exit(1);}if(dup2(fd,STDIN_FILENO)<0){perror("dup2");_exit(1);}close(fd);}
        if(argc>=4){int fd=open(argv[3],O_WRONLY|O_CREAT|O_TRUNC,0644);if(fd<0){perror(argv[3]);_exit(1);}if(dup2(fd,STDOUT_FILENO)<0){perror("dup2");_exit(1);}close(fd);}
        execvp(argv[1],&argv[1]);perror(argv[1]);_exit(127);
    }
    int st; if(waitpid(p,&st,0)<0){perror("waitpid");return 1;}return WIFEXITED(st)?WEXITSTATUS(st):1;
}
