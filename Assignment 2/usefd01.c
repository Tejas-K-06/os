#include <stdio.h>
#include <unistd.h>

int main() {
	char buffer[1024];
	int n = read(0, buffer, 1024);
	int ans = 0;
	for(int i = 0; i < n - 1; i++) {
		if (buffer[i] < '0' || buffer[i] > '9') {
        		printf("Invalid\n");
        		return -1;
    		}
		int temp = buffer[i] - '0';
		ans = ans * 10 + temp;
	}
	printf("%d\n", ans);
	//sleep(1);

	int x = 999666333;
	char buffer2[1024];
	int i = 0;
	while(x) {
		buffer2[i] = x % 10 + '0' ;
		x = x / 10;
		i++;
	}
	for(int t = i - 1; t >= 0; t--) {;
		write(1, &buffer2[t], 1);
	}
	return 0; 
}
