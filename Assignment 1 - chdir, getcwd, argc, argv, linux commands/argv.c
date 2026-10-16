#include <stdio.h>

int main(int arg_count, char** arg_variable) {
	for(int i = 0; i < arg_count; i++) {
		printf("%s ", *(arg_variable++));
	}	
	printf("\n");
       	return 0;
}       
