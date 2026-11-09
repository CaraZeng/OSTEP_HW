#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define BUCKETS 1024

typedef struct node {
    int key;
    struct node *next;
} node_t;

typedef struct {
    node_t *buckets[BUCKETS];
    pthread_mutex_t global_lock;
    pthread_mutex_t bucket_locks[BUCKETS];
} hash_table_t;

typedef struct {
    hash_table_t *table;
    int thread_id;
    int operations;
    int mode;
} thread_arg_t;

double now(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void table_init(hash_table_t *table) {
    for (int i = 0; i < BUCKETS; i++) {
        table->buckets[i] = NULL;
        pthread_mutex_init(&table->bucket_locks[i], NULL);
    }

    pthread_mutex_init(&table->global_lock, NULL);
}

void insert(hash_table_t *table, int key, int mode) {
    int index = key % BUCKETS;

    pthread_mutex_t *lock =
        mode == 0 ? &table->global_lock : &table->bucket_locks[index];

    pthread_mutex_lock(lock);

    node_t *node = malloc(sizeof(node_t));
    node->key = key;
    node->next = table->buckets[index];
    table->buckets[index] = node;

    pthread_mutex_unlock(lock);
}

void *worker(void *arg) {
    thread_arg_t *data = arg;

    for (int i = 0; i < data->operations; i++) {
        int key = data->thread_id * data->operations + i;
        insert(data->table, key, data->mode);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr,
                "Usage: %s <threads> <operations_per_thread> <mode>\n",
                argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int operations = atoi(argv[2]);
    int mode = atoi(argv[3]);

    if (n <= 0 || operations <= 0 || (mode != 0 && mode != 1)) {
        fprintf(stderr, "Invalid arguments.\n");
        return 1;
    }

    hash_table_t *table = malloc(sizeof(hash_table_t));
    table_init(table);

    pthread_t *threads = malloc(n * sizeof(pthread_t));
    thread_arg_t *args = malloc(n * sizeof(thread_arg_t));

    double start = now();

    for (int i = 0; i < n; i++) {
        args[i].table = table;
        args[i].thread_id = i;
        args[i].operations = operations;
        args[i].mode = mode;

        pthread_create(&threads[i], NULL, worker, &args[i]);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    double end = now();

    int count = 0;

    for (int i = 0; i < BUCKETS; i++) {
        node_t *curr = table->buckets[i];

        while (curr != NULL) {
            count++;
            node_t *next = curr->next;
            free(curr);
            curr = next;
        }

        pthread_mutex_destroy(&table->bucket_locks[i]);
    }

    pthread_mutex_destroy(&table->global_lock);

    printf("Threads: %d, Mode: %s, Count: %d, Time: %.6f seconds\n",
           n, mode == 0 ? "Global" : "Per-bucket",
           count, end - start);

    free(table);
    free(threads);
    free(args);

    return 0;
}