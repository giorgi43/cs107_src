/* testing sortedset */
#include "sortedset.h"

int cmp(const void* l, const void* r) {
	if ( *(double*) l < *(double*) r) return -1;
	else if ( *(double*) l == *(double*) r) return 0;
	else return 1;
}

int main() {
	double arr[3] = {34.3, 44.2, 10.9};

	sortedset* set = malloc(sizeof(sortedset));
	SetNew(set, sizeof(double), cmp);
	
	SetAdd(set, &arr[0]);
	SetAdd(set, &arr[1]);
	SetAdd(set, &arr[2]);
	
	//int a = 100;
	double* elem = (double*) SetSearch(set, &arr[1]);
	if (elem != NULL) {
		printf("element %f found\n", arr[1]);
	} else {
		printf("element %f not found\n", arr[1]);
	}

	free(set);
	return 0;
}
