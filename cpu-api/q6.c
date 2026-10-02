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
        printf("Child running\n");
        sleep(1);
        printf("Child finished\n");
        exit(0);
    } else {
        int status;

        pid_t result = waitpid(pid, &status, 0);

        printf("waitpid returned %d\n", result);
    }

    return 0;
}