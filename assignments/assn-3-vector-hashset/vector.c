#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* helper functions */

static void extendVector(vector* v) {
    if (v->allocLen == 0) v->allocLen++;
	v->allocLen *= 2; //+= v->initAllocLen;
	v->elems = realloc(v->elems, v->allocLen * v->elemSize);
	assert(v->elems != NULL);
}

/**********************/

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation) {
	// initialize members
	v->elemSize = elemSize;
	v->freeFn = freeFn;
	v->logLen = 0;
	assert(initialAllocation >= 0);
	v->allocLen = initialAllocation; //(initialAllocation >= 0) ? initialAllocation : DEFAULT_INIT_SIZE;
	v->initAllocLen = v->allocLen; // save to grow with initAllocLen segments? or *= 2
	// allocate memory
	v->elems = malloc(v->allocLen * elemSize);
	assert(v->elems != NULL);
}

void VectorDispose(vector *v) {
	if (v->freeFn != NULL) {
		for (int i = 0; i < v->logLen; i++) {
		    void* ptr = ((char*) v->elems) + i * v->elemSize;
			v->freeFn(ptr);
		}
	}
	free(v->elems);
}

int VectorLength(const vector *v) {
	return v->logLen;
}

void *VectorNth(const vector *v, int position) {
	assert(position >= 0 && position < v->logLen); // check bounds
	return ((char*) v->elems) + v->elemSize * position;
}

void VectorReplace(vector *v, const void *elemAddr, int position) {
	assert(position >= 0 && position < v->logLen); // check bounds
	void* dest = ((char*) v->elems) + position * v->elemSize;
	if (v->freeFn != NULL) {
	    v->freeFn(dest);
	}
	memcpy(dest, elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position) {
	assert(position >= 0 && position <= v->logLen); // check bounds
	if (v->logLen == v->allocLen) {
		extendVector(v);
	}
	int segmentSize = (v->logLen - position) * v->elemSize;
	void* src = ((char*) v->elems) + position * v->elemSize;
	void* dest = ((char*) src) + v->elemSize;
	memmove(dest, src, segmentSize);
	memcpy(src, elemAddr, v->elemSize);
	v->logLen++;
}

void VectorAppend(vector *v, const void *elemAddr) {
	if (v->logLen == v->allocLen) {
		extendVector(v);
	}
	void* back = ((char*) v->elems) + v->logLen * v->elemSize;
	memcpy(back, elemAddr, v->elemSize);
	v->logLen++;
}

void VectorDelete(vector *v, int position) {
	assert(position >= 0 && position < v->logLen); // check bounds
    void* elem = ((char*) v->elems) + position * v->elemSize;
	if (v->freeFn != NULL) {
		v->freeFn(elem);
	}
	int segmentSize = (v->logLen - position - 1) * v->elemSize; // size of buffer after position (excluding position)
	void* src = ((char*) v->elems) + (position+1) * v->elemSize;
	memmove(elem, src, segmentSize);
	v->logLen--;
}

void VectorSort(vector *v, VectorCompareFunction compare) {
	assert(compare != NULL);
	qsort(v->elems, v->logLen, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData) {
	assert(mapFn != NULL);
	char* ptr = (char*) v->elems;
	for (int i = 0; i < v->logLen; i++) {
		mapFn(ptr + i * v->elemSize, auxData);
	}
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted) {
    if (v->logLen == 0) return kNotFound;
    assert(startIndex >= 0 && startIndex < v->logLen && searchFn != NULL);
	void* start = ((char*) v->elems) + v->elemSize * startIndex;
	if (isSorted) {
		void* res = bsearch(key, start, v->logLen - startIndex, v->elemSize, searchFn);
		if (res == NULL) return kNotFound;
		return ((char*)res - (char*)v->elems) / v->elemSize;
	}
	// linear search
	// (alternative is lsearch() from search.h)
	char* ptr = (char*) start;
	for (int i = 0; i < v->logLen - startIndex; i++) {
		if (searchFn(key, ptr + i * v->elemSize) == 0) return i;
	}
	return kNotFound;
}
