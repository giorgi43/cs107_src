#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define ARR_LEN 40000
#define NUM_THREADS 4
#define ELEM_MAX_SIZE 1000

struct args_t {
	int* arr;
	int thread_id;
};

long init_array(int* arr) {
	long sum = 0;
	for (size_t i = 0; i < ARR_LEN; i++) {
		arr[i] = rand() % ELEM_MAX_SIZE;
		sum += arr[i];
	}
	return sum;
}

void* calculate_sum(void* args) {
	struct args_t* a = (struct args_t*) args;
	long* sum = malloc(sizeof(long));
	*sum = 0;
	size_t slice_len = (ARR_LEN-1)/NUM_THREADS + 1;
	for (size_t i = 0; i < slice_len; i++) {
		size_t index = a->thread_id * slice_len + i;
		if (index >= ARR_LEN) break;
		*sum += a->arr[index];
	}
	pthread_exit(sum);
}

int main() {
	srand(time(NULL));

	// create random array, calculate sum
	int* arr = malloc(sizeof(int) * ARR_LEN);
	assert(arr != NULL);
	long sum_for_check = init_array(arr);

	long sum = 0; // results returned from threads will add up to this variable

	// create array for threads and args
	pthread_t threads[NUM_THREADS];
	struct args_t args_arr[NUM_THREADS]; 

	// create threads
	for (int i = 0; i < NUM_THREADS; i++) {
		args_arr[i].arr = arr;
		args_arr[i].thread_id = i;
		pthread_create(&threads[i], NULL, calculate_sum, &args_arr[i]);
	}

	// wait for threads and store temporary result in slice_sum;
	void* slice_sum;
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], &slice_sum);
		sum += *(long*) slice_sum;
		free(slice_sum);
	}
	
	// print results, clean up heap
	printf("sum computed with single thread: %ld\n", sum_for_check);
	printf("sum computed with 4 threads: %ld\n", sum);
	free(arr);
	return 0;
}
