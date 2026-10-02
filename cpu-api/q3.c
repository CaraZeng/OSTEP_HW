#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // child
        close(pipefd[0]);

        printf("hello\n");

        // signal parent
        write(pipefd[1], "x", 1);
        close(pipefd[1]);
    } else {
        // parent
        char buffer;

        close(pipefd[1]);

        // blocks until child writes
        read(pipefd[0], &buffer, 1);

        printf("goodbye\n");

        close(pipefd[0]);
    }

    return 0;
}