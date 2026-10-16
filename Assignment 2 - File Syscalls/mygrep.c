#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

int customstrlen(char *str) {
        int i = 0;
        while(str[i] != '\0') i++;
        return i;
}

int customstrstr(char *str, char *word) {
        int i, j;
        for(i = 0; str[i] != '\0'; i++) {
                j = 0;
                while(word[j] != '\0' && str[i + j] == word[j]) {
                        j++;
                }
                if(word[j] == '\0') {
                        return 1;
                }
        }
        return 0;
}

int main(int argc, char *argv[]) {
        char *word = argv[2];
        int fd = open(argv[1], O_RDONLY);

        if(fd == -1) {
                perror("File Error");
                return errno;
        }

        char line[1024];
        int i = 0;
        char ch;

        while(read(fd, &ch, 1) > 0) {
                if(ch == '\n') {
                        line[i] = '\0';
                        if(customstrstr(line, word)) {
                                write(1, line, customstrlen(line));
                                write(1, "\n", 1);
                        }
                        i = 0;
                } else {
                        line[i++] = ch;
                }
        }

        if(i > 0) {
                line[i] = '\0';
                if(customstrstr(line, word)) {
                        write(1, line, customstrlen(line));
                        write(1, "\n", 1);
                }
        }
        
        close(fd);
        return 0;
}
