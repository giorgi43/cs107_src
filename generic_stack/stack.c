#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void StackInit(Stack* s, size_t elem_size, void (*free_fn)(void*)) {
	s->free_fn = free_fn;
	s->elem_size = elem_size;
	s->log_len = 0;
	s->alloc_len = 4;
	s->elems = malloc(s->alloc_len * elem_size);
	if (s->elems == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(1);
	}
}

void StackDispose(Stack* s) {
	// if user provided free_fn function
	if (s->free_fn != NULL) {
		for (size_t i = 0; i < s->log_len; i++) {
			s->free_fn( (char*) s->elems + i*s->elem_size);
		}
	}
	free(s->elems);
}

void StackPush(Stack* s, void* elem) {
	// check if reallocation is needed
	if (s->alloc_len == s->log_len) {
		s->alloc_len *= 2;
		s->elems = realloc(s->elems, s->alloc_len*s->elem_size);
		if (s->elems == NULL) {
			fprintf(stderr, "Memory reallocation failed\n");
			exit(1);
		}
	}
	void* new_ptr = (char*) s->elems + s->log_len * s->elem_size;
	memcpy(new_ptr, elem, s->elem_size);
	s->log_len++;
}

void StackPop(Stack* s, void* elem) {
	if (s->log_len == 0) {
		fprintf(stderr, "Can not pop. Stack is empty\n");
		return;
	}
	s->log_len--;
	void* top_ptr = (char*) s->elems + s->log_len * s->elem_size;
	memcpy(elem, top_ptr, s->elem_size);
}


