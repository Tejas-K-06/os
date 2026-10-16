/* Q10. Shell demonstrating parent-process built-ins cd and exit. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void){char line[512];while(fputs("mini> ",stdout),fflush(stdout),fgets(line,sizeof line,stdin)){
 line[strcspn(line,"\n")]=0;char*av[32];int n=0;char*save=NULL;char*t=strtok_r(line," \t",&save);while(t&&n<31){av[n++]=t;t=strtok_r(NULL," \t",&save);}av[n]=NULL;if(!n)continue;
 if(!strcmp(av[0],"exit"))break;if(!strcmp(av[0],"cd")){if(!av[1])fprintf(stderr,"cd: missing directory\n");else if(chdir(av[1])<0)perror("cd");continue;}
 pid_t p=fork();if(p<0){perror("fork");continue;}if(p==0){execvp(av[0],av);perror(av[0]);_exit(127);}waitpid(p,NULL,0);
 }return 0;}
