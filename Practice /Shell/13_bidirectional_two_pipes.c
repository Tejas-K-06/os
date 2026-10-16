#define _POSIX_C_SOURCE 200809L
/* Q13: Bidirectional parent-child communication requires two pipes. */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
static int write_all(int fd,const char *p,size_t n){while(n){ssize_t w=write(fd,p,n);if(w<0){if(errno==EINTR)continue;return -1;}p+=w;n-=(size_t)w;}return 0;}
int main(void){
 int pc[2],cp[2];if(pipe(pc)<0){perror("pipe parent-child");return 1;}if(pipe(cp)<0){perror("pipe child-parent");close(pc[0]);close(pc[1]);return 1;}
 pid_t p=fork();if(p<0){perror("fork");close(pc[0]);close(pc[1]);close(cp[0]);close(cp[1]);return 1;}
 if(p==0){close(pc[1]);close(cp[0]);char in[128];size_t used=0;ssize_t n=0;while(used<sizeof in-1&&(n=read(pc[0],in+used,sizeof in-1-used))>0)used+=(size_t)n;if(n<0){perror("read");_exit(1);}in[used]='\0';char out[160];int len=snprintf(out,sizeof out,"Child reply: received '%s'",in);int rc=(len<0||(size_t)len>=sizeof out||write_all(cp[1],out,(size_t)len)<0);close(pc[0]);close(cp[1]);_exit(rc?1:0);}
 close(pc[0]);close(cp[1]);const char msg[]="ping";int rc=write_all(pc[1],msg,sizeof msg);if(rc<0)perror("write");close(pc[1]);char reply[160];size_t used=0;ssize_t n;while(used<sizeof reply-1&&(n=read(cp[0],reply+used,sizeof reply-1-used))>0)used+=(size_t)n;if(n<0){perror("read");rc=-1;}reply[used]='\0';if(used)printf("%s\n",reply);close(cp[0]);int st;while(waitpid(p,&st,0)<0){if(errno==EINTR)continue;perror("waitpid");return 1;}return rc<0?1:(WIFEXITED(st)?WEXITSTATUS(st):1);
}
