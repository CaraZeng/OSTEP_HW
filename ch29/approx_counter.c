#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct {
    long long global;
    pthread_mutex_t global_lock;
    long long *local;
    pthread_mutex_t *local_locks;
    int threshold;
    int nthreads;
} counter_t;

typedef struct {
    counter_t *counter;
    int id;
    long long iterations;
} thread_arg_t;

void *worker(void *arg) {
    thread_arg_t *data = arg;
    counter_t *c = data->counter;
    int id = data->id;

    for (long long i = 0; i < data->iterations; i++) {
        pthread_mutex_lock(&c->local_locks[id]);

        c->local[id]++;

        if (c->local[id] >= c->threshold) {
            pthread_mutex_lock(&c->global_lock);
            c->global += c->local[id];
            pthread_mutex_unlock(&c->global_lock);

            c->local[id] = 0;
        }

        pthread_mutex_unlock(&c->local_locks[id]);
    }

    return NULL;
}

double now(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr,
                "Usage: %s <threads> <total_increments> <threshold>\n",
                argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    long long total = atoll(argv[2]);
    int threshold = atoi(argv[3]);

    if (n <= 0 || total <= 0 || threshold <= 0) {
        fprintf(stderr, "Arguments must be positive.\n");
        return 1;
    }

    counter_t c;
    c.global = 0;
    c.threshold = threshold;
    c.nthreads = n;

    pthread_mutex_init(&c.global_lock, NULL);

    c.local = calloc(n, sizeof(long long));
    c.local_locks = malloc(n * sizeof(pthread_mutex_t));

    pthread_t *tid = malloc(n * sizeof(pthread_t));
    thread_arg_t *args = malloc(n * sizeof(thread_arg_t));

    for (int i = 0; i < n; i++) {
        pthread_mutex_init(&c.local_locks[i], NULL);
    }

    double start = now();

    for (int i = 0; i < n; i++) {
        args[i].counter = &c;
        args[i].id = i;
        args[i].iterations = total / n + (i < total % n);

        pthread_create(&tid[i], NULL, worker, &args[i]);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(tid[i], NULL);
    }

    double end = now();

    // Flush remaining local counts to obtain the exact final value.
    for (int i = 0; i < n; i++) {
        c.global += c.local[i];
    }

    printf("Threads: %d, Threshold: %d, Count: %lld, Time: %.6f seconds\n",
           n, threshold, c.global, end - start);

    for (int i = 0; i < n; i++) {
        pthread_mutex_destroy(&c.local_locks[i]);
    }

    pthread_mutex_destroy(&c.global_lock);

    free(c.local);
    free(c.local_locks);
    free(tid);
    free(args);

    return 0;
}