#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t child1 = fork();

    if (child1 < 0) {
        perror("fork");
        exit(1);
    }

    if (child1 == 0) {
        // child 1

        close(pipefd[0]);

        // stdout -> pipe write end
        dup2(pipefd[1], STDOUT_FILENO);

        close(pipefd[1]);

        printf("hello from child 1\n");

        exit(0);
    }

    pid_t child2 = fork();

    if (child2 < 0) {
        perror("fork");
        exit(1);
    }

    if (child2 == 0) {
        // child 2

        close(pipefd[1]);

        // stdin <- pipe read end
        dup2(pipefd[0], STDIN_FILENO);

        close(pipefd[0]);

        char buffer[100];

        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            printf("Child 2 received: %s", buffer);
        }

        exit(0);
    }

    // parent doesn't use pipe
    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    return 0;
}