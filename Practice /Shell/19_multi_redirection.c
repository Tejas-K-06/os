/* Q19: One-command launcher with input/output redirection.
 * Usage: ./multi_redirect command [input-file [output-file]]
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc,char**argv){
 if(argc<2||argc>4){fprintf(stderr,"Usage: %s command [input-file [output-file]]\n",argv[0]);return 2;}pid_t p=fork();if(p<0){perror("fork");return 1;}
 if(p==0){
  if(argc>=3){int f=open(argv[2],O_RDONLY);if(f<0){perror(argv[2]);_exit(1);}if(dup2(f,STDIN_FILENO)<0){perror("dup2 stdin");close(f);_exit(1);}close(f);}
  if(argc>=4){int f=open(argv[3],O_WRONLY|O_CREAT|O_TRUNC,0644);if(f<0){perror(argv[3]);_exit(1);}if(dup2(f,STDOUT_FILENO)<0){perror("dup2 stdout");close(f);_exit(1);}close(f);}
  execvp(argv[1],&argv[1]);perror(argv[1]);_exit(errno==ENOENT?127:126);
 }
 int st;while(waitpid(p,&st,0)<0){if(errno==EINTR)continue;perror("waitpid");return 1;}return WIFEXITED(st)?WEXITSTATUS(st):(WIFSIGNALED(st)?128+WTERMSIG(st):1);
}
