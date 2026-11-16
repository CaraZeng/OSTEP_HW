#ifndef ZEMAPHORE_H
#define ZEMAPHORE_H

#include <pthread.h>

typedef struct {
    int value;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} Zem_t;

static void Zem_init(Zem_t *s, int value) {
    s->value = value;
    pthread_mutex_init(&s->lock, NULL);
    pthread_cond_init(&s->cond, NULL);
}

static void Zem_wait(Zem_t *s) {
    pthread_mutex_lock(&s->lock);

    while (s->value <= 0) {
        pthread_cond_wait(&s->cond, &s->lock);
    }

    s->value--;

    pthread_mutex_unlock(&s->lock);
}

static void Zem_post(Zem_t *s) {
    pthread_mutex_lock(&s->lock);

    s->value++;
    pthread_cond_signal(&s->cond);

    pthread_mutex_unlock(&s->lock);
}

static void Zem_destroy(Zem_t *s) {
    pthread_cond_destroy(&s->cond);
    pthread_mutex_destroy(&s->lock);
}

#endif