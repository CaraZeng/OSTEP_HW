#include <stdio.h>
#include <sys/time.h>

int main(void) {
    struct timeval start, end;

    long min_diff = 1000000;
    long max_diff = 0;
    int zero_count = 0;

    for (int i = 0; i < 1000000; i++) {
        gettimeofday(&start, NULL);
        gettimeofday(&end, NULL);

        long diff =
            (end.tv_sec - start.tv_sec) * 1000000L +
            (end.tv_usec - start.tv_usec);

        if (diff == 0) {
            zero_count++;
        } else {
            if (diff < min_diff) min_diff = diff;
            if (diff > max_diff) max_diff = diff;
        }
    }

    printf("Minimum nonzero interval: %ld microseconds\n", min_diff);
    printf("Maximum observed interval: %ld microseconds\n", max_diff);
    printf("Zero-difference measurements: %d / 1000000\n", zero_count);

    return 0;
}