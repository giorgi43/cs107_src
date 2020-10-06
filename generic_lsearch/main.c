#include <stdio.h>
#include <string.h>
#include "lsearch.h"

typedef struct {
	int x, y;
} Vector2d;

Vector2d newVec(int x, int y) {
	Vector2d vec;
	vec.x = x;
	vec.y = y;
	return vec;
}

// cmp_fn for integers
// return 0 if equal else -1
int cmp_fn_int(void* lhs, void* rhs) {
	if (*(int*) lhs == *(int*) rhs)
		return 0;
	return -1;
}

// cmp_fn for strings
int cmp_fn_str(void* lhs, void* rhs) {
	return strcmp(*(char**)lhs, *(char**)rhs);
}

// cmp_fn for Vector2d struct
// compare by magnitude
int cmp_fn_vector2d(void* lhs, void* rhs) {
	Vector2d* a = (Vector2d*)lhs;
	Vector2d* b = (Vector2d*)rhs;
	if (a->x*a->x + a->y*a->y == b->x*b->x + b->y*b->y) {
		return 0;
	}
	return -1;
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
		printf("integer %d not found\n", key_int);
	}

	/* testing strings */
	char* arr_strings[] = {"Hand me a knife", "slice", "dice", "afterlife"};
	char* key_str = "dice";
	char* found_str = (char*) lsearch(&key_str, arr_strings, 4, sizeof(char*), cmp_fn_str);
	if (found_str !=  NULL) {
		size_t index = (char**) found_str - arr_strings;
		printf("found \"%s\" at index %lu\n", key_str, index);
	} else {
		printf("string \"%s\" not found\n", key_str);
	}
	
	/* testing custom struct Vector2d */
	Vector2d arr_vecs[] = {newVec(2,4), newVec(100,23), newVec(54,-124), newVec(-12,-9)};
	Vector2d key_vec = newVec(-12, -9);
	Vector2d* found_vec = (Vector2d*) lsearch(&key_vec, arr_vecs, 4, sizeof(Vector2d), cmp_fn_vector2d);
	if (found_vec !=  NULL) {
		size_t index = (Vector2d*) found_vec - arr_vecs;
		printf("found vector at index %lu\n", index);
	} else {
		printf("vector not found\n");
	}

	return 0;
} 
