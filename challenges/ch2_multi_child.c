#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s N\n", argv[0]);
        return 1;
    }
    int N = atoi(argv[1]);
    if (N <= 0 || N > 254) {                // exit status is 0..255
        fprintf(stderr, "N must be 1..254\n");
        return 1;
    }

    for (int i = 0; i < N; i++) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); return 1; }
        if (pid == 0) {                     // child
            printf("child %d (PID=%d)\n", i, getpid());
            fflush(stdout);                 // _exit() won't flush stdio
            _exit(i + 1);
        }
    }

    for (int k = 0; k < N; k++) {           // parent: collect in finish order
        int status;
        pid_t w = wait(&status);
        if (w == -1) { perror("wait"); return 1; }
        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status); // code == index + 1
            printf("parent: child index %d (pid %d) exited with status %d\n",
                   code - 1, (int)w, code);
        }
    }
    return 0;
}