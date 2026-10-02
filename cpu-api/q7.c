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

        close(STDOUT_FILENO);

        printf("You should not see this\n");

        fflush(stdout);

        exit(0);
    } else {
        wait(NULL);
        printf("Parent can still print\n");
    }

    return 0;
}