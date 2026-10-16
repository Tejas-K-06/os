/* Q10: Shell demonstrating parent-process built-ins cd and exit. */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void){
 char line[512];while(fputs("mini> ",stdout),fflush(stdout),fgets(line,sizeof line,stdin)){
  if(!strchr(line,'\n')&&!feof(stdin)){int c;while((c=getchar())!='\n'&&c!=EOF){}fprintf(stderr,"Input too long.\n");continue;}
  line[strcspn(line,"\n")]=0;char *av[32];int n=0;char *save=NULL;for(char*t=strtok_r(line," \t",&save);t&&n<31;t=strtok_r(NULL," \t",&save))av[n++]=t;av[n]=NULL;if(!n)continue;
  if(!strcmp(av[0],"exit"))break;
  if(!strcmp(av[0],"cd")){const char *dest=av[1]?av[1]:getenv("HOME");if(!dest){fprintf(stderr,"cd: HOME is not set\n");continue;}if(av[2]){fprintf(stderr,"cd: too many arguments\n");continue;}if(chdir(dest)<0)perror("cd");continue;}
  pid_t p=fork();if(p<0){perror("fork");continue;}if(p==0){execvp(av[0],av);perror(av[0]);_exit(errno==ENOENT?127:126);}
  int st;while(waitpid(p,&st,0)<0){if(errno==EINTR)continue;perror("waitpid");break;}
 }return ferror(stdin)?1:0;
}
