/* Q8. Dynamic-length pipeline: enter one line, commands separated by |.
   This is a compact teaching implementation; quoting/escaped pipes are unsupported. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAXLINE 1024
#define MAXCMD 16
#define MAXARG 32
static int split(char*s,char*v[],int max,char d){int n=0;char*save=NULL;char delim[2]={d,0};char*t=strtok_r(s,delim,&save);while(t&&n<max){while(*t==' '||*t=='\t')t++;v[n++]=t;t=strtok_r(NULL,delim,&save);}return n;}
int main(void){char line[MAXLINE];while(fputs("pipe-shell> ",stdout),fflush(stdout),fgets(line,sizeof line,stdin)){
 line[strcspn(line,"\n")]=0;if(!strcmp(line,"exit"))break;char*cmd[MAXCMD];int n=split(line,cmd,MAXCMD,'|');if(n<1)continue;
 int fds[MAXCMD-1][2];pid_t p[MAXCMD];int made=0;
 for(int i=0;i<n-1;i++){if(pipe(fds[i])<0){perror("pipe");return 1;}made++;}
 int started=0;
 for(int i=0;i<n;i++){char*av[MAXARG];int ac=split(cmd[i],av,MAXARG-1,' ');av[ac]=NULL;if(!ac)continue;
  pid_t c=fork();if(c<0){perror("fork");break;}if(c==0){if(i)dup2(fds[i-1][0],0);if(i<n-1)dup2(fds[i][1],1);for(int j=0;j<made;j++){close(fds[j][0]);close(fds[j][1]);}execvp(av[0],av);perror(av[0]);_exit(127);}p[started++]=c;}
 for(int i=0;i<made;i++){close(fds[i][0]);close(fds[i][1]);}for(int i=0;i<started;i++)waitpid(p[i],NULL,0);
 }return 0;}
