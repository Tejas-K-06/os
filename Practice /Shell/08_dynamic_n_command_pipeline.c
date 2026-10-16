/* Q8: Interactive pipeline shell, up to 16 commands separated by |.
 * Simple whitespace parsing only; quotes and escaped pipes are unsupported.
 */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAXLINE 1024
#define MAXCMD 16
#define MAXARG 32
static int split(char*s,char*v[],int max,char d){int n=0;char delim[2]={d,0};char*save=NULL;for(char*t=strtok_r(s,delim,&save);t&&n<max;t=strtok_r(NULL,delim,&save)){while(*t==' '||*t=='\t')t++;if(*t)v[n++]=t;}return n;}
int main(void){
 char line[MAXLINE];while(fputs("pipe-shell> ",stdout),fflush(stdout),fgets(line,sizeof line,stdin)){
  if(!strchr(line,'\n')&&!feof(stdin)){int c;while((c=getchar())!='\n'&&c!=EOF){}fprintf(stderr,"Input line too long.\n");continue;}
  line[strcspn(line,"\n")]=0;if(!strcmp(line,"exit"))break;if(!*line)continue;
  char *cmd[MAXCMD];int n=split(line,cmd,MAXCMD,'|');if(!n)continue;
  char *av[MAXCMD][MAXARG];int valid=1;
  for(int i=0;i<n;i++){int ac=split(cmd[i],av[i],MAXARG-1,' ');av[i][ac]=NULL;if(!ac){fprintf(stderr,"Empty command in pipeline.\n");valid=0;}}
  if(!valid)continue;
  int fds[MAXCMD-1][2],made=0,failed=0;
  for(int i=0;i<n-1;i++){if(pipe(fds[i])<0){perror("pipe");failed=1;break;}made++;}
  if(failed){for(int i=0;i<made;i++){close(fds[i][0]);close(fds[i][1]);}continue;}
  pid_t p[MAXCMD];int started=0;
  for(int i=0;i<n;i++){pid_t c=fork();if(c<0){perror("fork");failed=1;break;}if(c==0){
    if((i&&dup2(fds[i-1][0],STDIN_FILENO)<0)||(i<n-1&&dup2(fds[i][1],STDOUT_FILENO)<0)){perror("dup2");_exit(126);}
    for(int j=0;j<made;j++){close(fds[j][0]);close(fds[j][1]);}execvp(av[i][0],av[i]);perror(av[i][0]);_exit(errno==ENOENT?127:126);
   }p[started++]=c;}
  for(int i=0;i<made;i++){close(fds[i][0]);close(fds[i][1]);}
  int result=0;for(int i=0;i<started;i++){int st;while(waitpid(p[i],&st,0)<0){if(errno==EINTR)continue;perror("waitpid");result=1;break;}if(WIFEXITED(st)&&WEXITSTATUS(st))result=WEXITSTATUS(st);else if(WIFSIGNALED(st))result=128+WTERMSIG(st);}
  if(failed)fprintf(stderr,"Pipeline could not be started completely.\n");else if(result)fprintf(stderr,"Pipeline exit status: %d\n",result);
 }return ferror(stdin)?1:0;
}
