#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#define ITERATIONS 1000000

double get_time_us() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000000.0 + t.tv_usec;
}

int main() {
    int pipefd[2];
    char buf;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    double start = get_time_us();

    for (int i = 0; i < ITERATIONS; i++) {
        read(pipefd[0], &buf, 0);
    }

    double end = get_time_us();

    double total = end - start;
    double average = total / ITERATIONS;

    printf("Iterations: %d\n", ITERATIONS);
    printf("Total time: %.2f us\n", total);
    printf("Average system call time: %.4f us\n", average);
    printf("Average system call time: %.2f ns\n", average * 1000);

    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}