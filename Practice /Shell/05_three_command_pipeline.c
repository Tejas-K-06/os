/* Q5. Fixed three-command pipeline: cmd1 | cmd2 | cmd3.
   Compile: gcc -Wall -Wextra -o three_pipe 05_three_command_pipeline.c
   Run: ./three_pipe "cat file.txt" "grep COEP" "wc -l" */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
static int parse(char*s,char*a[],int cap){int n=0;char*save=NULL,*t=strtok_r(s," \t",&save);while(t&&n<cap-1){a[n++]=t;t=strtok_r(NULL," \t",&save);}a[n]=NULL;return n;}
int main(int argc,char**argv){
 if(argc!=4){fprintf(stderr,"Usage: %s 'cmd1' 'cmd2' 'cmd3'\n",argv[0]);return 2;}
 char b[3][512];char **av[3];char *args[3][32];
 for(int i=0;i<3;i++){snprintf(b[i],sizeof b[i],"%s",argv[i+1]);av[i]=args[i];if(!parse(b[i],av[i],32))return 2;}
 int p1[2],p2[2];if(pipe(p1)<0||pipe(p2)<0){perror("pipe");return 1;}
 pid_t pids[3];
 for(int i=0;i<3;i++){pid_t p=fork();if(p<0){perror("fork");return 1;}if(p==0){
   if(i==0)dup2(p1[1],1);else if(i==1){dup2(p1[0],0);dup2(p2[1],1);}else dup2(p2[0],0);
   close(p1[0]);close(p1[1]);close(p2[0]);close(p2[1]);execvp(av[i][0],av[i]);perror(av[i][0]);_exit(127);
 }pids[i]=p;}
 close(p1[0]);close(p1[1]);close(p2[0]);close(p2[1]);for(int i=0;i<3;i++)waitpid(pids[i],NULL,0);return 0;
}
