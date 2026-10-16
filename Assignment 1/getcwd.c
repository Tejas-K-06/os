#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	const char* path = argv[1];
	char buffer[1024];
	char* buf = getcwd(buffer, 1024);
	printf("Your Current Working Directory is: %s", buf);
	printf("\n");
	int n = chdir(path);
	char buffer2[1024];
	char* buf2 = getcwd(buffer2, 1024);
	printf("Your changed directory is: %s", buf2);
	printf("\n");
	return 0;
}
