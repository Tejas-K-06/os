/* Q13. Bidirectional parent-child communication requires two pipes. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void){int pc[2],cp[2];if(pipe(pc)<0||pipe(cp)<0){perror("pipe");return 1;}pid_t p=fork();if(p<0){perror("fork");return 1;}
 if(p==0){char in[128];close(pc[1]);close(cp[0]);ssize_t n=read(pc[0],in,sizeof in-1);if(n<0)_exit(1);in[n]=0;char out[160];snprintf(out,sizeof out,"Child reply: received '%s'",in);write(cp[1],out,strlen(out)+1);close(pc[0]);close(cp[1]);_exit(0);}
 close(pc[0]);close(cp[1]);const char msg[]="ping";write(pc[1],msg,sizeof msg);close(pc[1]);char reply[160];ssize_t n=read(cp[0],reply,sizeof reply-1);if(n>0){reply[n]=0;printf("%s\n",reply);}close(cp[0]);waitpid(p,NULL,0);return 0;}
