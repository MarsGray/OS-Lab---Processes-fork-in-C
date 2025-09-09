#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        // Only give the worker a minimal custom environment:
        // (exactly what we specify here)
        char *const envp[] = { "MYVAR=hello", NULL };

        // Args for the worker (argv[0] conventionally the program name)
        // Feel free to change/extend these.
        const char *w0 = "worker";
        const char *a1 = "one";
        const char *a2 = "two";

        // Run worker from the project root (./bin/worker).
        // If you run this binary from inside ./bin, the fallback "./worker" will work.
        execle("./bin/worker", w0, a1, a2, (char *)NULL, envp);

        // Fallbacks for different working directories:
        execle("bin/worker",  w0, a1, a2, (char *)NULL, envp);
        execle("./worker",    w0, a1, a2, (char *)NULL, envp);

        // If we get here, all execs failed.
        perror("execle worker");
        _exit(127);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) == -1) { perror("waitpid"); return 1; }

    if (WIFEXITED(status)) {
        printf("parent: worker (pid %d) finished with status %d\n", pid, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("parent: worker (pid %d) terminated by signal %d\n", pid, WTERMSIG(status));
    } else {
        printf("parent: worker (pid %d) ended abnormally\n", pid);
    }
    return 0;
}