#include <stdio.h>
#include "stack.h"

int main(int argc, char** argv) {
	/* test ints */
	Stack stack_ints;
	StackInit(&stack_ints, sizeof(int), NULL); // no free_fn for ints
	for (int i = 0; i < 8; i++) {
		StackPush(&stack_ints, &i);
	}
	int top;
	for (int i = 8; i != 0; i--) {
		StackPop(&stack_ints, &top);
		printf("Top element is %d\n", top);	
	}
	// stack is empty. try to pop
	StackPop(&stack_ints, &top);
	StackDispose(&stack_ints);

	return 0;
}
