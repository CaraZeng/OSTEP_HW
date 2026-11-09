#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct node {
    int key;
    struct node *next;
    pthread_mutex_t lock;
} node_t;

typedef struct {
    node_t *head;
    pthread_mutex_t global_lock;
} list_t;

typedef struct {
    list_t *list;
    int operations;
    int use_hand_over_hand;
} thread_arg_t;

double now(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

node_t *create_node(int key) {
    node_t *node = malloc(sizeof(node_t));
    node->key = key;
    node->next = NULL;
    pthread_mutex_init(&node->lock, NULL);
    return node;
}

void list_init(list_t *list, int size) {
    list->head = create_node(-1);
    pthread_mutex_init(&list->global_lock, NULL);

    node_t *curr = list->head;

    for (int i = 0; i < size; i++) {
        curr->next = create_node(i);
        curr = curr->next;
    }
}

int search_global(list_t *list, int key) {
    pthread_mutex_lock(&list->global_lock);

    node_t *curr = list->head->next;

    while (curr != NULL) {
        if (curr->key == key) {
            pthread_mutex_unlock(&list->global_lock);
            return 1;
        }
        curr = curr->next;
    }

    pthread_mutex_unlock(&list->global_lock);
    return 0;
}

int search_hand_over_hand(list_t *list, int key) {
    node_t *curr = list->head;
    pthread_mutex_lock(&curr->lock);

    while (curr->next != NULL) {
        node_t *next = curr->next;
        pthread_mutex_lock(&next->lock);
        pthread_mutex_unlock(&curr->lock);

        curr = next;

        if (curr->key == key) {
            pthread_mutex_unlock(&curr->lock);
            return 1;
        }
    }

    pthread_mutex_unlock(&curr->lock);
    return 0;
}

void *worker(void *arg) {
    thread_arg_t *data = arg;

    for (int i = 0; i < data->operations; i++) {
        int key = i % 1000;

        if (data->use_hand_over_hand) {
            search_hand_over_hand(data->list, key);
        } else {
            search_global(data->list, key);
        }
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

    list_t list;
    list_init(&list, 1000);

    pthread_t *threads = malloc(n * sizeof(pthread_t));
    thread_arg_t *args = malloc(n * sizeof(thread_arg_t));

    double start = now();

    for (int i = 0; i < n; i++) {
        args[i].list = &list;
        args[i].operations = operations;
        args[i].use_hand_over_hand = mode;

        pthread_create(&threads[i], NULL, worker, &args[i]);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    double end = now();

    printf("Threads: %d, Mode: %s, Time: %.6f seconds\n",
           n, mode ? "Hand-over-hand" : "Global",
           end - start);

    free(threads);
    free(args);

    node_t *curr = list.head;
    while (curr != NULL) {
        node_t *next = curr->next;
        pthread_mutex_destroy(&curr->lock);
        free(curr);
        curr = next;
    }
    pthread_mutex_destroy(&list.global_lock);

    return 0;
}