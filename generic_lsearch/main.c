#include <stdio.h>
#include "lsearch.h"

// cmp_fn for integers
// return 0 if equal else -1
int cmp_fn(void* lhs, void* rhs) {
	if (*(int*) lhs == *(int*) rhs)
		return 0;
	return -1;
}

int main(int argc, char** argv) {
	/* testing integers */
	int arr_ints[] = {12,34,55,124,93,1010,53};
	int key_int = 1010;
	int* ptr_ints = (int*) lsearch(&key_int, arr_ints, sizeof(arr_ints)/sizeof(arr_ints[0]), sizeof(arr_ints[0]), cmp_fn);
	if (ptr_ints !=  NULL) {
		unsigned long index = ptr_ints - arr_ints;
		printf("found %d at index %lu\n", key_int, index);
	} else {
		printf("element %d not found\n", key_int);
	}

	return 0;
} 
