#include "sortedset.h"

#define NodeSize(clientElem)  ((clientElem) + 2 * sizeof(int))

static const int kInitialCapacity = 4;
void SetNew(sortedset *set, int elemSize, int (*cmpfn) (const void *, const void *)) {
	assert(elemSize > 0);
	assert(cmpfn != NULL);
	set->root = malloc(sizeof(int) + kInitialCapacity*NodeSize(elemSize));
	if (set->root == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(1);
	}
	set->cmp = cmpfn;
	set->logicalSize = 0;
	set->allocatedSize = kInitialCapacity;
	set->elemSize = elemSize;

	set->root[0] = -1; // initially empty
}

static int* findNode(sortedset *set, const void *elem) {
	void* current; // current element being examined
	int comp; // current comparison function result
	int* root = set->root; 
	while (*root != -1) {
		//        0th element address     +    index where to jump
		current = (char*) (set->root + 1) + *root * NodeSize(set->elemSize);
		// compare if current element equal to elem
		comp = set->cmp(elem, current);
		if (comp == 0) break;
		// decide where to go - (left or right)
		root = (int*) ((char*) current + set->elemSize); // left one
		if (comp > 0) root++; // if elem was bigger go to right
	}
	return root;
}

static void expand(sortedset* set) {
	set->allocatedSize *= 2;
	set->root = realloc(set->root, sizeof(int) + set->allocatedSize*NodeSize(set->elemSize));
	if (set->root == NULL) {
		fprintf(stderr, "Memory reallocation failed");
		exit(1);
	}
} 

bool SetAdd(sortedset *set, const void *elemPtr) {
	int* i = findNode(set, elemPtr);
	if (*i != -1) return false; // already in set
	if (set->logicalSize == set->allocatedSize) expand(set);
	*i = set->logicalSize++;
	void* newNode = (char*) (set->root+1) + *i * NodeSize(set->elemSize);
	memcpy(newNode, elemPtr, set->elemSize);
	
	i = (int*) ((char*) newNode + set->elemSize);
	i[0] = i[1] = -1;
	return true;
}

void *SetSearch(sortedset *set, const void *elemPtr) {
	int* i = findNode(set, elemPtr);
	if (*i == -1) {
		return NULL;
	}
	return (char*) (set->root + 1) + *i * NodeSize(set->elemSize);
}
