#include <stdio.h>
#include "swap.h"

int main(int argc, char** argv) {
	/* swap ints */
	int a = 5;
	int b = 44;
	printf("before swapping: a = %d b = %d\n", a, b);
	swap(&a, &b, sizeof(int));
	printf("after swapping: a = %d b = %d\n", a, b);

	/* swap strings */
	char* str1 = "byte beat";
	char* str2 = "Bigfoot";
	printf("before swapping: str1 = \"%s\" str2 = \"%s\"\n", str1, str2);
	swap(&str1, &str2, sizeof(char*));	
	printf("after swapping: str1 = \"%s\" str2 = \"%s\"\n", str1, str2);

	return 0;
}
