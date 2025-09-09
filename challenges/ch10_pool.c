#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s M task1 [task2 ...]\n", argv[0]);
        return 1;
    }

    int M = atoi(argv[1]);
    if (M <= 0) { fprintf(stderr, "M must be > 0\n"); return 1; }

    int N = argc - 2, next = 0, running = 0;
    if (M > N) M = N;

    while (next < N || running > 0) {
        // Launch new children while under the concurrency limit
        while (running < M && next < N) {
            const char *task = argv[2 + next];
            pid_t pid = fork();
            if (pid < 0) { perror("fork"); return 1; }
            if (pid == 0) { // child
                printf("[child %d] start '%s'\n", getpid(), task);
                fflush(stdout);
                sleep(1); // simulate work
                printf("[child %d] done  '%s'\n", getpid(), task);
                _exit(0);
            }
            // parent
            printf("[parent %d] started child %d for '%s'\n", getpid(), pid, task);
            running++;
            next++;
        }

        // At limit or no new tasks -> wait for one child to finish
        if (running > 0) {
            int status;
            pid_t w = waitpid(-1, &status, 0);
            if (w > 0) {
                printf("[parent %d] reaped child %d (status=%d)\n",
                       getpid(), (int)w, WIFEXITED(status) ? WEXITSTATUS(status) : -1);
                running--;
            }
        }
    }
    return 0;
}