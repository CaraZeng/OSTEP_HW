#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#ifdef __linux__
#define _GNU_SOURCE
#include <sched.h>
#endif
static double seconds(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return t.tv_sec + t.tv_nsec * 1e-9; }
int main(int argc, char **argv) {
    if (argc != 3) { fprintf(stderr, "usage: %s pages trials\n", argv[0]); return 1; }
    long pages = atol(argv[1]), trials = atol(argv[2]);
    if (pages <= 0 || trials <= 0 || pages > 1000000) return 1;
    long ps = sysconf(_SC_PAGESIZE); size_t stride = (size_t)ps / sizeof(uint32_t);
    uint32_t *a = calloc((size_t)pages * stride, sizeof(*a));
    if (!a) return 1;
    for (long i = 0; i < pages; ++i) a[i * stride] = (uint32_t)i;
    volatile uint32_t *v = a;
    double start = seconds();
    for (long t = 0; t < trials; ++t)
        for (long i = 0; i < pages; ++i) v[i * stride] += 1;
    double elapsed = seconds() - start;
    printf("%ld,%ld,%.4f\n", pages, trials, elapsed * 1e9 / ((double)pages * trials));
    free(a); return 0;
}
