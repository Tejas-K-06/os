/* Q11. Fixed pipeline: cat FILE | grep PATTERN | head -N */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc,char**argv){if(argc!=4){fprintf(stderr,"Usage: %s file pattern N\n",argv[0]);return 2;}
 int a[2],b[2];if(pipe(a)<0||pipe(b)<0){perror("pipe");return 1;}pid_t p[3];
 for(int i=0;i<3;i++){pid_t c=fork();if(c<0){perror("fork");return 1;}if(c==0){
  if(i==0){dup2(a[1],1);close(a[0]);close(a[1]);close(b[0]);close(b[1]);char*av[]={"cat",argv[1],NULL};execvp(av[0],av);}
  if(i==1){dup2(a[0],0);dup2(b[1],1);close(a[0]);close(a[1]);close(b[0]);close(b[1]);char*av[]={"grep",argv[2],NULL};execvp(av[0],av);}
  dup2(b[0],0);close(a[0]);close(a[1]);close(b[0]);close(b[1]);char*av[]={"head","-n",argv[3],NULL};execvp(av[0],av);
  perror("exec");_exit(127);
 }p[i]=c;}
 close(a[0]);close(a[1]);close(b[0]);close(b[1]);for(int i=0;i<3;i++)waitpid(p[i],NULL,0);return 0;}
