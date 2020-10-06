#include <stdio.h>
#include <string.h>
#include "lsearch.h"

// cmp_fn for integers
// return 0 if equal else -1
int cmp_fn_int(void* lhs, void* rhs) {
	if (*(int*) lhs == *(int*) rhs)
		return 0;
	return -1;
}

int cmp_fn_str(void* lhs, void* rhs) {
	return strcmp(*(char**)lhs, *(char**)rhs);
}

int main(int argc, char** argv) {
	/* testing integers */
	int arr_ints[] = {12,34,55,124,93,1010,53};
	int key_int = 1010;
	int* found_int = (int*) lsearch(&key_int, arr_ints, sizeof(arr_ints)/sizeof(arr_ints[0]), sizeof(arr_ints[0]), cmp_fn_int);
	if (found_int !=  NULL) {
		size_t index = found_int - arr_ints;
		printf("found %d at index %lu\n", key_int, index);
	} else {
		printf("element %d not found\n", key_int);
	}

	/* testing strings */
	char* arr_strings[] = {"Hand me a knife", "slice", "dice", "afterlife"};
	char* key_str = "dice";
	char* found_str = (char*) lsearch(&key_str, arr_strings, 4, sizeof(char*), cmp_fn_str);
	if (found_str !=  NULL) {
		size_t index = (char**) found_str - arr_strings;
		printf("found \"%s\" at index %lu\n", key_str, index);
	} else {
		printf("element \"%s\" not found\n", key_str);
	}

	return 0;
} 
