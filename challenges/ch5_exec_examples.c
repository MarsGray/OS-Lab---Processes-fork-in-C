#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t a = fork();
    if (a < 0) { perror("fork A"); return 1; }
    if (a == 0) {
        // Child A: execl
        execl("/bin/echo", "echo", "one", "two", (char *)NULL);
        perror("execl echo"); _exit(127);
    }

    pid_t b = fork();
    if (b < 0) { perror("fork B"); return 1; }
    if (b == 0) {
        // Child B: execv
        char *const args[] = { "echo", "one", "two", NULL };
        execv("/bin/echo", args);
        perror("execv echo"); _exit(127);
    }

    // Parent: wait for both
    int status;
    if (waitpid(a, &status, 0) == -1) { perror("waitpid A"); return 1; }
    if (waitpid(b, &status, 0) == -1) { perror("waitpid B"); return 1; }

    return 0;
}