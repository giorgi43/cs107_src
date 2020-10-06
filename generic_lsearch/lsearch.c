#include "lsearch.h"

void* lsearch(void* key, void* arr, size_t arr_size,
				size_t elem_size, int (*cmp_fn) (void*, void*)) 
{
	for (size_t i = 0; i < arr_size; i++) {
		void* elem_ptr = (char*) (arr + elem_size*i);
		if (cmp_fn(key, elem_ptr) == 0) {
			return elem_ptr;
		}
	}
	return NULL;
}


