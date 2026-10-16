/* Q14: Producer child writes integers; parent consumes them from a pipe. */
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void){
 int fd[2];if(pipe(fd)<0){perror("pipe");return 1;}pid_t p=fork();if(p<0){perror("fork");close(fd[0]);close(fd[1]);return 1;}
 if(p==0){close(fd[0]);for(int i=1;i<=5;i++){size_t left=sizeof i;const char *ptr=(const char*)&i;while(left){ssize_t w=write(fd[1],ptr,left);if(w<0){if(errno==EINTR)continue;_exit(1);}ptr+=w;left-=(size_t)w;}}close(fd[1]);_exit(0);}
 close(fd[1]);int x;unsigned char *ptr=(unsigned char*)&x;size_t have=0;ssize_t n;int result=0;
 while((n=read(fd[0],ptr+have,sizeof x-have))>0){have+=(size_t)n;if(have==sizeof x){printf("Consumed: %d\n",x);have=0;ptr=(unsigned char*)&x;}}
 if(n<0){perror("read");result=1;}if(have){fprintf(stderr,"Incomplete integer received.\n");result=1;}close(fd[0]);int st;while(waitpid(p,&st,0)<0){if(errno==EINTR)continue;perror("waitpid");return 1;}return result||( !WIFEXITED(st)||WEXITSTATUS(st));
}
