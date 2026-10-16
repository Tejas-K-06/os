/* Q5: Run cmd1 | cmd2 | cmd3.
 * Command strings use simple whitespace parsing; shell quoting is not interpreted.
 * Example: ./three_pipe "cat file.txt" "grep COEP" "wc -l"
 */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
static int parse(char*s,char*a[],int cap){int n=0;char*save=NULL;for(char*t=strtok_r(s," \t",&save);t&&n<cap-1;t=strtok_r(NULL," \t",&save))a[n++]=t;a[n]=NULL;return n;}
static int wait_status(pid_t p){int st;while(waitpid(p,&st,0)<0){if(errno==EINTR)continue;perror("waitpid");return 1;}return WIFEXITED(st)?WEXITSTATUS(st):(WIFSIGNALED(st)?128+WTERMSIG(st):1);}
int main(int argc,char**argv){
 if(argc!=4){fprintf(stderr,"Usage: %s 'cmd1' 'cmd2' 'cmd3'\n",argv[0]);return 2;}
 char b[3][512];char *av[3][32];
 for(int i=0;i<3;i++){if(strlen(argv[i+1])>=sizeof b[i]){fprintf(stderr,"Command too long.\n");return 2;}strcpy(b[i],argv[i+1]);if(!parse(b[i],av[i],32)){fprintf(stderr,"Empty command.\n");return 2;}}
 int p1[2],p2[2];if(pipe(p1)<0){perror("pipe");return 1;}if(pipe(p2)<0){perror("pipe");close(p1[0]);close(p1[1]);return 1;}
 pid_t pids[3];int started=0;
 for(int i=0;i<3;i++){pid_t p=fork();if(p<0){perror("fork");break;}if(p==0){
  if((i==1&&dup2(p1[0],STDIN_FILENO)<0)||(i==2&&dup2(p2[0],STDIN_FILENO)<0)||(i==0&&dup2(p1[1],STDOUT_FILENO)<0)||(i==1&&dup2(p2[1],STDOUT_FILENO)<0)){perror("dup2");_exit(126);}
  close(p1[0]);close(p1[1]);close(p2[0]);close(p2[1]);execvp(av[i][0],av[i]);perror(av[i][0]);_exit(errno==ENOENT?127:126);
 }pids[started++]=p;}
 close(p1[0]);close(p1[1]);close(p2[0]);close(p2[1]);int result=0;for(int i=0;i<started;i++){int s=wait_status(pids[i]);if(s)result=s;}return started==3?result:1;
}
