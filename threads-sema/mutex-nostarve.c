#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "zemaphore.h"

#define MAX_THREADS 100

typedef struct __ns_mutex_t {
    pthread_mutex_t guard;
    unsigned long next_ticket;
    unsigned long now_serving;
    Zem_t waiting[MAX_THREADS];
} ns_mutex_t;

void ns_mutex_init(ns_mutex_t *m) {
    pthread_mutex_init(&m->guard, NULL);
    m->next_ticket = 0;
    m->now_serving = 0;

    for (int i = 0; i < MAX_THREADS; i++)
        Zem_init(&m->waiting[i], 0);
}

void ns_mutex_acquire(ns_mutex_t *m) {
    pthread_mutex_lock(&m->guard);

    unsigned long ticket = m->next_ticket++;

    if (ticket == m->now_serving) {
        pthread_mutex_unlock(&m->guard);
        return;
    }

    pthread_mutex_unlock(&m->guard);

    Zem_wait(&m->waiting[ticket % MAX_THREADS]);
}

void ns_mutex_release(ns_mutex_t *m) {
    pthread_mutex_lock(&m->guard);

    m->now_serving++;

    if (m->now_serving < m->next_ticket)
        Zem_post(&m->waiting[m->now_serving % MAX_THREADS]);

    pthread_mutex_unlock(&m->guard);
}

void ns_mutex_destroy(ns_mutex_t *m) {
    for (int i = 0; i < MAX_THREADS; i++)
        Zem_destroy(&m->waiting[i]);

    pthread_mutex_destroy(&m->guard);
}

ns_mutex_t mutex;
int counter = 0;
int loops = 5;

void *worker(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < loops; i++) {
        ns_mutex_acquire(&mutex);

        counter++;
        printf("thread %d: counter = %d\n", id, counter);

        ns_mutex_release(&mutex);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    int n = 5;

    if (argc > 1)
        n = atoi(argv[1]);

    assert(n > 0 && n <= MAX_THREADS);

    pthread_t threads[n];
    int ids[n];

    ns_mutex_init(&mutex);

    printf("parent: begin\n");

    for (int i = 0; i < n; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < n; i++)
        pthread_join(threads[i], NULL);

    printf("parent: end, counter = %d\n", counter);

    ns_mutex_destroy(&mutex);

    return 0;
}
