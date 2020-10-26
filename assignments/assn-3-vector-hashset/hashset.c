#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
    assert(elemSize > 0);
    assert(numBuckets > 0);
    assert(hashfn != NULL);
    assert(comparefn != NULL);
    h->elemSize = elemSize;
    h->numBuckets = numBuckets;
    h->numElements = 0;
    h->cmpFn = comparefn;
    h->freeFn = freefn;
    h->hashFn = hashfn;
    h->buckets = malloc(sizeof(vector) * numBuckets);
    assert(h->buckets != NULL);
    for (int i = 0; i < numBuckets; i++) {
        VectorNew(h->buckets + i, elemSize, freefn, 0);
    }
}

void HashSetDispose(hashset *h) {
    for (int i = 0; i < h->numBuckets; i++) {
        VectorDispose(h->buckets + i);
    }
    free(h->buckets);
}

int HashSetCount(const hashset *h) { return h->numElements; }

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData) {
    assert(mapfn != NULL);
    for (int i = 0; i < h->numBuckets; i++) {
        VectorMap(h->buckets + i, mapfn, auxData);
    }
}

void HashSetEnter(hashset *h, const void *elemAddr) {
    assert(elemAddr != NULL);
    int bucketIndex = h->hashFn(elemAddr, h->numBuckets); // hash
    assert(bucketIndex >= 0 && bucketIndex < h->numBuckets);
    vector* bucket = h->buckets + bucketIndex;
    int index = VectorSearch(bucket, elemAddr, h->cmpFn, 0, false);
    if (index == -1) {
        VectorAppend(bucket, elemAddr);
    } else {
        VectorReplace(bucket, elemAddr, index);
    }
    h->numElements++;
}

void *HashSetLookup(const hashset *h, const void *elemAddr) {
    assert(elemAddr != NULL);
    int bucketIndex = h->hashFn(elemAddr, h->numBuckets);
    assert(bucketIndex >= 0 && bucketIndex < h->numBuckets);
    vector* bucket = h->buckets + bucketIndex;
    int index = VectorSearch(bucket, elemAddr, h->cmpFn, 0, false);
    return (index == -1) ? NULL : VectorNth(bucket, index);
}
