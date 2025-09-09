#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int immediate = (argc > 1 && strcmp(argv[1], "immediate") == 0);

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        // Child: exit right away -> becomes zombie until parent waits
        printf("[child ] PID=%d, PPID=%d: exiting now\n", getpid(), getppid());
        fflush(stdout);
        _exit(0);
    } else {
        // Parent
        printf("[parent] PID=%d: spawned child PID=%d\n", getpid(), pid);
        if (!immediate) {
            printf("[parent] Sleeping 10s. In another terminal run:\n");
            printf("         ps -l | grep zombie_wait_demo\n");
            printf("         Look for state 'Z' (zombie) and 'defunct' CMD.\n\n");
            fflush(stdout);
            sleep(10);
        }
        int status = 0;
        waitpid(pid, &status, 0);
        printf("[parent] waitpid() reaped child %d (status=%d). Done.\n",
               pid, WIFEXITED(status) ? WEXITSTATUS(status) : -1);
    }
    return 0;
}