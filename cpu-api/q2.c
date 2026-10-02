#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(void) {
    int fd = open("q2-output.txt",
                  O_CREAT | O_WRONLY | O_TRUNC,
                  0644);

    if (fd < 0) {
        perror("open");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        write(fd, "child\n", 6);
        close(fd);
    } else {
        write(fd, "parent\n", 7);
        wait(NULL);
        close(fd);
    }

    return 0;
}