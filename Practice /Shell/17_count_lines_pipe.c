/* Q17: Child writes text; parent counts newline characters read from pipe. */
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
static int write_all(int fd,const char *p,size_t n){while(n){ssize_t w=write(fd,p,n);if(w<0){if(errno==EINTR)continue;return -1;}p+=w;n-=(size_t)w;}return 0;}
int main(void){
 int fd[2];if(pipe(fd)<0){perror("pipe");return 1;}pid_t p=fork();if(p<0){perror("fork");close(fd[0]);close(fd[1]);return 1;}
 if(p==0){close(fd[0]);const char msg[]="first line\nsecond line\nthird line\n";int rc=write_all(fd[1],msg,sizeof msg-1);close(fd[1]);_exit(rc<0?1:0);}
 close(fd[1]);char b[128];ssize_t n;long lines=0;while((n=read(fd[0],b,sizeof b))>0)for(ssize_t i=0;i<n;i++)if(b[i]=='\n')lines++;if(n<0){perror("read");close(fd[0]);return 1;}close(fd[0]);int st;while(waitpid(p,&st,0)<0){if(errno==EINTR)continue;perror("waitpid");return 1;}printf("Line count: %ld\n",lines);return WIFEXITED(st)?WEXITSTATUS(st):1;
}
