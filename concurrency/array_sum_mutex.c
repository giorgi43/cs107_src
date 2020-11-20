#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#define ARR_LEN 40000
#define NUM_THREADS 4
#define ELEM_MAX_SIZE 1000

struct arg_t {
	long* sum;
	int* arr;
	pthread_mutex_t* mutex;
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

void* calculate_sum(void* arg) {
	struct arg_t* a = (struct arg_t*) arg;
	size_t slice_len = ARR_LEN/NUM_THREADS;
	for (size_t i = 0; i < slice_len; i++) {
		pthread_mutex_lock(a->mutex);
		*a->sum += a->arr[a->thread_id * slice_len + i];
		pthread_mutex_unlock(a->mutex);
	}
	return NULL;
}

int main() {
	srand(time(NULL));

	// create random array, calculate sum
	int* arr = malloc(sizeof(int) * ARR_LEN);
	assert(arr != NULL);
	long sum_for_check = init_array(arr);
	
	long sum = 0;
	// allocate memory for mutex and initialize mutex
	pthread_mutex_t* lock = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(lock, NULL);

	// create array for threads and args for runner function
	pthread_t threads[NUM_THREADS];
	struct arg_t args_arr[NUM_THREADS];

	// create threads
	for (int i = 0; i < NUM_THREADS; i++) {
		args_arr[i].sum = &sum;
		args_arr[i].arr = arr;
		args_arr[i].mutex = lock;
		args_arr[i].thread_id = i;
		pthread_create(&threads[i], NULL, calculate_sum, &(args_arr[i]));
	}
	
	// wait for threads
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	
	// print results and clean up heap
	printf("sum computed with single thread: %ld\n", sum_for_check);
	printf("sum computed with 4 threads: %ld\n", sum);
	pthread_mutex_destroy(lock); 
	free(lock);
	free(arr);
	return 0;
}
