#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

long long counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    long long iterations;
} thread_arg_t;

void *worker(void *arg) {
    thread_arg_t *data = (thread_arg_t *)arg;

    for (long long i = 0; i < data->iterations; i++) {
        pthread_mutex_lock(&lock);
        counter++;
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

double now(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <threads> <total_increments>\n", argv[0]);
        return 1;
    }

    int threads = atoi(argv[1]);
    long long total = atoll(argv[2]);

    if (threads <= 0 || total <= 0) {
        fprintf(stderr, "Arguments must be positive.\n");
        return 1;
    }

    pthread_t *tid = malloc(threads * sizeof(pthread_t));
    thread_arg_t *args = malloc(threads * sizeof(thread_arg_t));

    double start = now();

    for (int i = 0; i < threads; i++) {
        args[i].iterations =
            total / threads + (i < total % threads);

        pthread_create(&tid[i], NULL, worker, &args[i]);
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(tid[i], NULL);
    }

    double end = now();

    printf("Threads: %d, Count: %lld, Time: %.6f seconds\n",
           threads, counter, end - start);

    free(tid);
    free(args);
    pthread_mutex_destroy(&lock);

    return 0;
}