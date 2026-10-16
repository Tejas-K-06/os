#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char cmd[128], *args[32], *t;
    char aliasName[32] = "", aliasVal[64] = "";
    int i, pid;

    //trying to handle a single alias
    while (1) {
        printf("prompt> ");
        
	if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
            break; 
        }
        cmd[strcspn(cmd, "\n")] = '\0';
        
	//handling alias name=value 
        if (strncmp(cmd, "alias ", 6) == 0) {
            char *eq = strchr(cmd + 6, '=');
            if (eq) {
                *eq = '\0';
                strcpy(aliasName, cmd + 6);
                strcpy(aliasVal, eq + 1);
            }
            continue;
        }

        i = 0;
        t = strtok(cmd, " ");
        while (t != NULL && i < 31) {
            args[i++] = t;
            t = strtok(NULL, " ");
        }
        args[i] = NULL;

       //comparing the alias and using it 
        if (strcmp(args[0], aliasName) == 0) {
            args[0] = strtok(aliasVal, " ");
        }

        pid = fork();
        if (pid < 0) {
            perror("fork failed"); 
        } else if (pid == 0) {
            execvp(args[0], args); //execvp is more useful 
            perror("exec failed"); 
            exit(1);
        } else {
            wait(NULL);
        }
    }
    return 0;
}




