#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        printf("Child: pid = %d\n", getpid());

        pid_t result = wait(NULL);

        printf("Child wait returned: %d\n", result);
        perror("child wait");

    } else {
        int status;

        pid_t result = wait(&status);

        printf("Parent: wait returned %d\n", result);
    }

    return 0;
}