/* Q4: Equivalent to cat input.txt | grep pattern > output.txt.
 * Usage: ./pipe_redirect input.txt pattern output.txt
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
static int wait_status(pid_t p){int st;while(waitpid(p,&st,0)<0){if(errno==EINTR)continue;perror("waitpid");return 1;}return WIFEXITED(st)?WEXITSTATUS(st):(WIFSIGNALED(st)?128+WTERMSIG(st):1);}
int main(int argc,char **argv){
    if(argc!=4){fprintf(stderr,"Usage: %s input pattern output\n",argv[0]);return 2;}
    int fd[2];if(pipe(fd)<0){perror("pipe");return 1;}
    pid_t a=fork();if(a<0){perror("fork");close(fd[0]);close(fd[1]);return 1;}
    if(a==0){int in=open(argv[1],O_RDONLY);if(in<0){perror(argv[1]);_exit(1);}if(dup2(in,STDIN_FILENO)<0||dup2(fd[1],STDOUT_FILENO)<0){perror("dup2");_exit(126);}close(in);close(fd[0]);close(fd[1]);char *av[]={"cat",NULL};execvp(av[0],av);perror("cat");_exit(127);}
    pid_t b=fork();if(b<0){perror("fork");close(fd[0]);close(fd[1]);wait_status(a);return 1;}
    if(b==0){int out=open(argv[3],O_WRONLY|O_CREAT|O_TRUNC,0644);if(out<0){perror(argv[3]);_exit(1);}if(dup2(fd[0],STDIN_FILENO)<0||dup2(out,STDOUT_FILENO)<0){perror("dup2");_exit(126);}close(out);close(fd[0]);close(fd[1]);char *av[]={"grep",argv[2],NULL};execvp(av[0],av);perror("grep");_exit(127);}
    close(fd[0]);close(fd[1]);int sa=wait_status(a),sb=wait_status(b);return sb?sb:sa;
}
