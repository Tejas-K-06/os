/* Q12. Execute a command and report its exit status. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc,char**argv){if(argc<2){fprintf(stderr,"Usage: %s command [args...]\n",argv[0]);return 2;}
 pid_t p=fork();if(p<0){perror("fork");return 1;}if(p==0){execvp(argv[1],&argv[1]);perror(argv[1]);_exit(127);}
 int st;if(waitpid(p,&st,0)<0){perror("waitpid");return 1;}if(WIFEXITED(st))printf("Exit status: %d\n",WEXITSTATUS(st));else if(WIFSIGNALED(st))printf("Terminated by signal: %d\n",WTERMSIG(st));return 0;}
