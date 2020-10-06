#ifndef SWAP_H
#define SWAP_H
#include <stddef.h>

/*
*  swap elements in memory
*  @param a pointer to first element
*  @param b pointer to second element
*  @param elem_size size of element in bytes
*/
void swap(void* a, void* b, size_t elem_size);

#endif
