#define _POSIX_C_SOURCE 200809L
/* Q6: Parent sends a message to child through a pipe. */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
static int write_all(int fd,const char *p,size_t n){while(n){ssize_t w=write(fd,p,n);if(w<0){if(errno==EINTR)continue;return -1;}p+=w;n-=(size_t)w;}return 0;}
int main(void){
 int fd[2];if(pipe(fd)<0){perror("pipe");return 1;}pid_t p=fork();if(p<0){perror("fork");close(fd[0]);close(fd[1]);return 1;}
 if(p==0){char buf[128];close(fd[1]);ssize_t n;size_t used=0;while(used<sizeof buf-1&&(n=read(fd[0],buf+used,sizeof buf-1-used))>0)used+=(size_t)n;if(n<0){perror("read");close(fd[0]);_exit(1);}buf[used]='\0';close(fd[0]);dprintf(STDOUT_FILENO,"Child received: %s\n",buf);_exit(0);}
 close(fd[0]);const char msg[]="Hello from parent";int err=write_all(fd[1],msg,sizeof msg);if(err<0)perror("write");close(fd[1]);int st;while(waitpid(p,&st,0)<0){if(errno==EINTR)continue;perror("waitpid");return 1;}return err<0?1:(WIFEXITED(st)?WEXITSTATUS(st):1);
}
