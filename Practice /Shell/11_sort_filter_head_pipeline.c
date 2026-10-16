/* Q11: Fixed pipeline: cat FILE | grep PATTERN | head -n N. */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
static int wait_status(pid_t p){int st;while(waitpid(p,&st,0)<0){if(errno==EINTR)continue;perror("waitpid");return 1;}return WIFEXITED(st)?WEXITSTATUS(st):(WIFSIGNALED(st)?128+WTERMSIG(st):1);}
int main(int argc,char**argv){
 if(argc!=4){fprintf(stderr,"Usage: %s file pattern N\n",argv[0]);return 2;}char *end=NULL;errno=0;long count=strtol(argv[3],&end,10);if(errno||!end||*end||count<0){fprintf(stderr,"N must be a non-negative integer.\n");return 2;}
 int a[2],b[2];if(pipe(a)<0){perror("pipe");return 1;}if(pipe(b)<0){perror("pipe");close(a[0]);close(a[1]);return 1;}pid_t p[3];int started=0;
 for(int i=0;i<3;i++){pid_t c=fork();if(c<0){perror("fork");break;}if(c==0){
  if((i==0&&dup2(a[1],STDOUT_FILENO)<0)||(i==1&&dup2(a[0],STDIN_FILENO)<0)||(i==1&&dup2(b[1],STDOUT_FILENO)<0)||(i==2&&dup2(b[0],STDIN_FILENO)<0)){perror("dup2");_exit(126);}
  close(a[0]);close(a[1]);close(b[0]);close(b[1]);
  if(i==0){char *av[]={"cat",argv[1],NULL};execvp(av[0],av);perror("cat");}
  else if(i==1){char *av[]={"grep",argv[2],NULL};execvp(av[0],av);perror("grep");}
  else {char *av[]={"head","-n",argv[3],NULL};execvp(av[0],av);perror("head");}
  _exit(127);
 }p[started++]=c;}
 close(a[0]);close(a[1]);close(b[0]);close(b[1]);int result=0;for(int i=0;i<started;i++){int s=wait_status(p[i]);if(s)result=s;}return started==3?result:1;
}
