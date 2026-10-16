/* Q19. One-command shell with < and > redirection.
   Usage: ./multi_redirect command [input-file] [output-file] */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc,char**argv){if(argc<2||argc>4){fprintf(stderr,"Usage: %s command [input-file [output-file]]\n",argv[0]);return 2;}pid_t p=fork();if(p<0){perror("fork");return 1;}if(p==0){
 if(argc>=3){int f=open(argv[2],O_RDONLY);if(f<0){perror(argv[2]);_exit(1);}if(dup2(f,0)<0){perror("dup2");_exit(1);}close(f);}
 if(argc>=4){int f=open(argv[3],O_WRONLY|O_CREAT|O_TRUNC,0644);if(f<0){perror(argv[3]);_exit(1);}if(dup2(f,1)<0){perror("dup2");_exit(1);}close(f);}
 execvp(argv[1],&argv[1]);perror(argv[1]);_exit(127);}int st;if(waitpid(p,&st,0)<0){perror("waitpid");return 1;}return WIFEXITED(st)?WEXITSTATUS(st):1;}
