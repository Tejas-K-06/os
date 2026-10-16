/* Q9: Small shell with PATH=dir:dir support. Commands containing / are executed directly. */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define LINE 512
static char path[LINE]="/usr/bin:/bin";
int main(void){
 char line[LINE];while(fputs("path-shell> ",stdout),fflush(stdout),fgets(line,sizeof line,stdin)){
  if(!strchr(line,'\n')&&!feof(stdin)){int c;while((c=getchar())!='\n'&&c!=EOF){}fprintf(stderr,"Input too long.\n");continue;}
  line[strcspn(line,"\n")]=0;if(!strcmp(line,"exit"))break;
  if(!strncmp(line,"PATH=",5)){if(strlen(line+5)>=sizeof path){fprintf(stderr,"PATH too long.\n");continue;}strcpy(path,line+5);continue;}
  char *av[32];int n=0;char *save=NULL;for(char*t=strtok_r(line," \t",&save);t&&n<31;t=strtok_r(NULL," \t",&save))av[n++]=t;av[n]=NULL;if(!n)continue;
  pid_t p=fork();if(p<0){perror("fork");continue;}if(p==0){
   if(strchr(av[0],'/')){execv(av[0],av);perror(av[0]);_exit(errno==ENOENT?127:126);}
   char copy[LINE];strcpy(copy,path);/* Empty PATH components mean the current directory. */
   char *cur=copy;
   for(;;){char *colon=strchr(cur,':');if(colon)*colon='\0';const char *dir=*cur?cur:".";char full[LINE];int k=snprintf(full,sizeof full,"%s/%s",dir,av[0]);if(k>0&&(size_t)k<sizeof full)execv(full,av);
    if(!colon) break;
    cur=colon+1;}
   perror(av[0]);_exit(errno==ENOENT?127:126);
  }
  int st;while(waitpid(p,&st,0)<0){if(errno==EINTR)continue;perror("waitpid");break;}
 }return 0;
}
