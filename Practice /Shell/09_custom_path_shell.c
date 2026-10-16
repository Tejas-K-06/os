/* Q9. Small shell with PATH=dir:dir support and execvp-style lookup.
   Commands with / are executed directly; otherwise search custom PATH. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define LINE 512
static char path[LINE]="/usr/bin:/bin";
int main(void){char line[LINE];while(fputs("path-shell> ",stdout),fflush(stdout),fgets(line,sizeof line,stdin)){
 line[strcspn(line,"\n")]=0;if(!strcmp(line,"exit"))break;if(!strncmp(line,"PATH=",5)){snprintf(path,sizeof path,"%s",line+5);continue;}
 char*av[32];int n=0;char*save=NULL;char*t=strtok_r(line," \t",&save);while(t&&n<31){av[n++]=t;t=strtok_r(NULL," \t",&save);}av[n]=NULL;if(!n)continue;
 pid_t p=fork();if(p<0){perror("fork");continue;}if(p==0){if(strchr(av[0],'/')){execv(av[0],av);perror(av[0]);_exit(127);}
 char copy[LINE];snprintf(copy,sizeof copy,"%s",path);char*sv=NULL;char*d=strtok_r(copy,":",&sv);while(d){char full[LINE];snprintf(full,sizeof full,"%s/%s",d,av[0]);execv(full,av);d=strtok_r(NULL,":",&sv);}perror(av[0]);_exit(127);}
 waitpid(p,NULL,0);
 }return 0;}
