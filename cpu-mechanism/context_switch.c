#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sched.h>

#define ITERATIONS 100000

double get_time_us() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000000.0 + t.tv_usec;
}

void pin_to_cpu(int cpu) {
    cpu_set_t set;

    CPU_ZERO(&set);
    CPU_SET(cpu, &set);

    if (sched_setaffinity(0, sizeof(set), &set) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }
}

int main() {
    int p2c[2];
    int c2p[2];

    if (pipe(p2c) == -1 || pipe(c2p) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* Child */

        pin_to_cpu(0);

        close(p2c[1]);
        close(c2p[0]);

        char byte;

        for (int i = 0; i < ITERATIONS; i++) {
            read(p2c[0], &byte, 1);
            write(c2p[1], &byte, 1);
        }

        close(p2c[0]);
        close(c2p[1]);

        exit(0);

    } else {
        /* Parent */

        pin_to_cpu(0);

        close(p2c[0]);
        close(c2p[1]);

        char byte = 'x';

        double start = get_time_us();

        for (int i = 0; i < ITERATIONS; i++) {
            write(p2c[1], &byte, 1);
            read(c2p[0], &byte, 1);
        }

        double end = get_time_us();

        wait(NULL);

        double total = end - start;

        /*
         * Each round trip causes approximately two
         * context switches:
         *
         * parent -> child
         * child  -> parent
         */
        double average =
            total / (2.0 * ITERATIONS);

        printf("Iterations: %d\n", ITERATIONS);
        printf("Total time: %.2f us\n", total);
        printf("Estimated context switch time: %.4f us\n",
               average);
        printf("Estimated context switch time: %.2f ns\n",
               average * 1000);

        close(p2c[1]);
        close(c2p[0]);
    }

    return 0;
}