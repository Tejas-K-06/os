/* Q15: Parent reads a file and sends bytes to child via a pipe.
 * Run: ./file_to_pipe input.txt
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
static int write_all(int fd,const char *p,size_t n){while(n){ssize_t w=write(fd,p,n);if(w<0){if(errno==EINTR)continue;return -1;}p+=w;n-=(size_t)w;}return 0;}
int main(int argc,char**argv){
 if(argc!=2){fprintf(stderr,"Usage: %s file\n",argv[0]);return 2;}int f=open(argv[1],O_RDONLY);if(f<0){perror(argv[1]);return 1;}int fd[2];if(pipe(fd)<0){perror("pipe");close(f);return 1;}
 pid_t p=fork();if(p<0){perror("fork");close(f);close(fd[0]);close(fd[1]);return 1;}
 if(p==0){close(fd[1]);char b[256];ssize_t n;int rc=0;while((n=read(fd[0],b,sizeof b))>0)if(write_all(STDOUT_FILENO,b,(size_t)n)<0){perror("write stdout");rc=1;break;}if(n<0){perror("read pipe");rc=1;}close(fd[0]);_exit(rc);}
 close(fd[0]);char b[256];ssize_t n;int rc=0;while((n=read(f,b,sizeof b))>0)if(write_all(fd[1],b,(size_t)n)<0){perror("write pipe");rc=1;break;}if(n<0){perror("read file");rc=1;}close(f);close(fd[1]);int st;while(waitpid(p,&st,0)<0){if(errno==EINTR)continue;perror("waitpid");return 1;}return rc||!WIFEXITED(st)||WEXITSTATUS(st);
}
