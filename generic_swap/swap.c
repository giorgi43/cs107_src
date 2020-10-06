#include "swap.h"
#include <string.h>

void swap(void* a, void* b, size_t elem_size) {
	char tmp[elem_size]; // this buffer can hold "elem_size" object
	memcpy(tmp, a, elem_size);
	memcpy(a, b, elem_size);
	memcpy(b, tmp, elem_size);
}
