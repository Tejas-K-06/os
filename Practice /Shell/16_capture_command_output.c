/* Q16: Parent captures a command's stdout through a pipe and relays it to stdout.
 * Run: ./capture_output ls -l
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
static int write_all(int fd,const char *p,size_t n){while(n){ssize_t w=write(fd,p,n);if(w<0){if(errno==EINTR)continue;return -1;}p+=w;n-=(size_t)w;}return 0;}
int main(int argc,char**argv){
 if(argc<2){fprintf(stderr,"Usage: %s command [args...]\n",argv[0]);return 2;}int fd[2];if(pipe(fd)<0){perror("pipe");return 1;}
 pid_t p=fork();if(p<0){perror("fork");close(fd[0]);close(fd[1]);return 1;}
 if(p==0){close(fd[0]);if(dup2(fd[1],STDOUT_FILENO)<0){perror("dup2");_exit(126);}close(fd[1]);execvp(argv[1],&argv[1]);perror(argv[1]);_exit(errno==ENOENT?127:126);}
 close(fd[1]);char b[256];ssize_t n;int rc=0;while((n=read(fd[0],b,sizeof b))>0)if(write_all(STDOUT_FILENO,b,(size_t)n)<0){perror("write stdout");rc=1;break;}if(n<0){perror("read pipe");rc=1;}close(fd[0]);int st;while(waitpid(p,&st,0)<0){if(errno==EINTR)continue;perror("waitpid");return 1;}if(rc)return rc;return WIFEXITED(st)?WEXITSTATUS(st):(WIFSIGNALED(st)?128+WTERMSIG(st):1);
}
