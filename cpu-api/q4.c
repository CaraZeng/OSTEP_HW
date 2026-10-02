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

        printf("Child is about to run ls\n");

        execl("/bin/ls", "ls", "-l", NULL);

        // only runs if exec fails
        perror("execl");
        exit(1);

    } else {
        wait(NULL);
        printf("Parent finished\n");
    }

    return 0;
}