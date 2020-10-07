#ifndef STACK_H
#define STACK_H

#include <stddef.h>

typedef struct {
	void* elems;
	size_t elem_size;
	size_t log_len;
	size_t alloc_len;
	void (*free_fn) (void*); // client should provide function to free up elements
} Stack;

// Initialize new stack, provide information about size of elements
// free up routine
void StackInit(Stack* s, size_t elem_size, void (*free_fn)(void*));

// "Destructor"
void StackDispose(Stack* s);

// Push new element to the stack
void StackPush(Stack* s, void* elem);

// Pop top element and store in elem
void StackPop(Stack* s, void* elem);

#endif
