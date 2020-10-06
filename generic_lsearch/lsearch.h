#ifndef LSEARCH_H
#define LSEARCH_H
#include <stddef.h>

/*
*  searches element in array by iterating from left to right
*  and returns pointer to this element. returns NULL if search fails
*  @param key element to find
*  @param arr array in which we search for element
*  @param arr_size number of elements in array
*  @param elem_size size of element in bytes
*  @param cmp_fn(void*, void*) comparison function
*/
void* lsearch(void* key, void* arr, size_t arr_size, size_t elem_size,
								int (*cmp_fn) (void*, void*));


#endif
