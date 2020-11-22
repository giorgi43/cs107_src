#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include "sha256.h"

#define NUM_THREADS 4
#define HASH_SIZE 32

void set_to_memory(uint8_t* hash, const char* input) {
    char byte_tmp[3];
    byte_tmp[2] = '\0';
    for (unsigned short i = 0, j = 0; i < 63; i+=2, j++) {
        byte_tmp[0] = input[i];
        byte_tmp[1] = input[i+1];
        uint8_t byte = (uint8_t) strtol(byte_tmp, NULL, 16);
        hash[j] = byte;
    }
}

char** read_passwords(FILE* fp, size_t* len) {
    size_t alloc_len = 1024;
    size_t log_len = 0;
    char** passwords = malloc(sizeof(char*) * alloc_len);
    char line[64];
    while (fgets(line, sizeof(line), fp)) {
        if (log_len == alloc_len) {
            alloc_len *= 2;
            passwords = realloc(passwords, sizeof(char*) * alloc_len);
            assert(passwords != NULL);
        }
        passwords[log_len] = strdup(line);
        log_len++;
    }
    *len = log_len;
    return passwords;
}

struct args_data {
    char** passwords;
    size_t starting_index;
    size_t segment_size;
    uint8_t* original_hash;
    char** result;
	bool* found;
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void* process_thread(void* arg) {
    struct args_data* args = (struct args_data*) arg;
    uint8_t hash_tmp[HASH_SIZE];
    for (size_t i = args->starting_index; i < args->starting_index + args->segment_size; i++) {
        if (*args->found) {
            pthread_exit(NULL);
        }
        const char* pass = args->passwords[i];
        sha256(pass, strlen(pass)-1, hash_tmp);
        if (memcmp(args->original_hash, hash_tmp, HASH_SIZE) == 0) {
			pthread_mutex_lock(&mutex);
            *args->result = args->passwords[i];
            //printf("Password found: %s", args->result);
            *args->found = true;
			pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s path_to_rockyou.txt hash\n", argv[0]);
        exit(1);
    }
    // hash to be cracked in string hex form
    const char* input = argv[2];//"9c192f9859ee2d18b12c1f61568a874b211ae21bfa52fd9162c32be6ef4eb4f1";
    uint8_t hash_original[HASH_SIZE]; // actual memory representation to use for comparing
    set_to_memory(hash_original, input);

    printf("Loading wordlist into memory\n");
    FILE* file = fopen(argv[1], "rb");
    if (file == NULL) {
        fprintf(stderr, "File could not be opened\n");
        exit(1);
    }
    size_t len;
    char** passwords = read_passwords(file, &len);
    fclose(file);
    printf("Done\n");

	bool found = false;
    char* hash_value = NULL;

    pthread_t threads[NUM_THREADS];             // hardcoded for demo
    struct args_data args_arr[NUM_THREADS] = {{passwords, 0, 3586098, hash_original, &hash_value, &found},
                                    {passwords, 3586098, 3586098, hash_original, &hash_value, &found},
                                    {passwords, 7172196, 3586098, hash_original, &hash_value, &found},
                                    {passwords, 10758294, 3586100, hash_original, &hash_value, &found}};

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, process_thread, &args_arr[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

	if (found) {
		printf("hash value is: %s", hash_value);
	} else {
		printf("no success\n");
	}

    for (size_t i = 0; i < len; i++) {
        free(passwords[i]);
    }
    free(passwords);
    return 0;
}
