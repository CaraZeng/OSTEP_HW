#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "common_threads.h"
#include "zemaphore.h"

Zem_t s;

void *child(void *arg) {
    sleep(1);
    printf("child\n");

    Zem_post(&s);

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;

    printf("parent: begin\n");

    Zem_init(&s, 0);

    Pthread_create(&p, NULL, child, NULL);

    Zem_wait(&s);

    printf("parent: end\n");

    Pthread_join(p, NULL);
    Zem_destroy(&s);

    return 0;
}