#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define LINES 10
#define BUFFER_SIZE 512

int main(int argc, char *argv[]) {
    int fd;
    char buffer[BUFFER_SIZE];
    int n, i;
    int lines = 0;

    if(argc != 2) {
        printf(2, "Usage: head <filename>\n");
        exit();
    }

    fd = open(argv[1], O_RDONLY);

    if(fd < 0) {
        printf(2, "head: cannot open %s\n", argv[1]);
        exit();
    }

    while(lines < LINES && (n = read(fd, buffer, sizeof(buffer))) > 0) {
        for(i = 0; i < n; i++) {
            printf(1, "%c", buffer[i]);
            if(buffer[i] == '\n') {
                lines++;
                if (lines == LINES) break;
            }
        }
    }

    close(fd);
    exit();
}