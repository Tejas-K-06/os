/* Q12: Execute a command and report its exit status. */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc,char**argv){
 if(argc<2){fprintf(stderr,"Usage: %s command [args...]\n",argv[0]);return 2;}
 pid_t p=fork();if(p<0){perror("fork");return 1;}if(p==0){execvp(argv[1],&argv[1]);perror(argv[1]);_exit(errno==ENOENT?127:126);}
 int st;while(waitpid(p,&st,0)<0){if(errno==EINTR)continue;perror("waitpid");return 1;}
 if(WIFEXITED(st)){printf("Exit status: %d\n",WEXITSTATUS(st));return WEXITSTATUS(st);}
 if(WIFSIGNALED(st)){printf("Terminated by signal: %d\n",WTERMSIG(st));return 128+WTERMSIG(st);}return 1;
}
