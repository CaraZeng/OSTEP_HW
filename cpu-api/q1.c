#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    int x = 100;

    printf("Before fork: x = %d, pid = %d\n", x, getpid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // child
        printf("Child: initial x = %d\n", x);
        x = 200;
        printf("Child: changed x = %d\n", x);
    } else {
        // parent
        printf("Parent: initial x = %d\n", x);
        x = 300;
        printf("Parent: changed x = %d\n", x);
    }

    return 0;
}