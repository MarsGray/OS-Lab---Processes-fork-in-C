#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {  // child
        printf("Hello from child (PID=%d, PPID=%d)\n", getpid(), getppid());
        fflush(stdout);
        sleep(2);
        _exit(7);
    } else {         // parent
        int status = 0;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }
        if (WIFEXITED(status)) {
            printf("child %d exited with status %d\n", pid, WEXITSTATUS(status));
        } else {
            printf("child %d did not exit normally\n", pid);
        }
    }
    return 0;
}