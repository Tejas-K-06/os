/* Q20: Demonstrates why every process must close unused pipe ends.
 * The parent closes both ends before waiting, so wc receives EOF correctly.
 */
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
static int wait_status(pid_t p){int st;while(waitpid(p,&st,0)<0){if(errno==EINTR)continue;perror("waitpid");return 1;}return WIFEXITED(st)?WEXITSTATUS(st):(WIFSIGNALED(st)?128+WTERMSIG(st):1);}
int main(void){
 int fd[2];if(pipe(fd)<0){perror("pipe");return 1;}pid_t a=fork();if(a<0){perror("fork");close(fd[0]);close(fd[1]);return 1;}
 if(a==0){if(dup2(fd[1],STDOUT_FILENO)<0){perror("dup2");_exit(126);}close(fd[0]);close(fd[1]);char *av[]={"printf","hello\n",NULL};execvp(av[0],av);perror("printf");_exit(127);}
 pid_t b=fork();if(b<0){perror("fork");close(fd[0]);close(fd[1]);wait_status(a);return 1;}
 if(b==0){if(dup2(fd[0],STDIN_FILENO)<0){perror("dup2");_exit(126);}close(fd[0]);close(fd[1]);char *av[]={"wc","-c",NULL};execvp(av[0],av);perror("wc");_exit(127);}
 close(fd[0]);close(fd[1]);int sa=wait_status(a),sb=wait_status(b);return sb?sb:sa;
}
