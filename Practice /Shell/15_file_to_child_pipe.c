/* Q15. Parent reads a file and sends bytes to child via a pipe.
   Run: ./file_to_pipe input.txt */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc,char**argv){if(argc!=2){fprintf(stderr,"Usage: %s file\n",argv[0]);return 2;}int f=open(argv[1],O_RDONLY);if(f<0){perror(argv[1]);return 1;}int pfd[2];if(pipe(pfd)<0){perror("pipe");close(f);return 1;}
 pid_t p=fork();if(p<0){perror("fork");close(f);return 1;}if(p==0){char b[256];close(pfd[1]);ssize_t n;while((n=read(pfd[0],b,sizeof b))>0)if(write(STDOUT_FILENO,b,(size_t)n)!=n)_exit(1);close(pfd[0]);_exit(n<0?1:0);}
 close(pfd[0]);char b[256];ssize_t n;while((n=read(f,b,sizeof b))>0){ssize_t off=0;while(off<n){ssize_t w=write(pfd[1],b+off,(size_t)(n-off));if(w<0){perror("write");close(f);close(pfd[1]);waitpid(p,NULL,0);return 1;}off+=w;}}
 close(f);close(pfd[1]);waitpid(p,NULL,0);return n<0?1:0;}
