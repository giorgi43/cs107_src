#include <stdio.h>
#include "swap.h"

int main(int argc, char** argv) {
	/* swap ints */
	int a = 5;
	int b = 44;
	printf("before swapping: a = %d, b = %d\n", a, b);
	swap(&a, &b, sizeof(int));
	printf("after swapping: a = %d, b = %d\n", a, b);

	return 0;
}
