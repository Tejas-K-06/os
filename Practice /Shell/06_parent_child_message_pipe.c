/* Q6. Parent sends a message to child through a pipe. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void){
 int fd[2];if(pipe(fd)<0){perror("pipe");return 1;}
 pid_t p=fork();if(p<0){perror("fork");return 1;}
 if(p==0){char buf[128];close(fd[1]);ssize_t n=read(fd[0],buf,sizeof(buf)-1);if(n<0){perror("read");_exit(1);}buf[n]='\0';printf("Child received: %s\n",buf);close(fd[0]);_exit(0);}
 close(fd[0]);const char msg[]="Hello from parent";if(write(fd[1],msg,sizeof(msg))<0)perror("write");close(fd[1]);waitpid(p,NULL,0);return 0;
}
