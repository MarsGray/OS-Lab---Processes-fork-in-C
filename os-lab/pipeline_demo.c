#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    const char *pattern = (argc > 1) ? argv[1] : ".c";
    int p[2];

    if (pipe(p) == -1) { perror("pipe"); return 1; }

    pid_t c1 = fork();
    if (c1 == -1) { perror("fork"); return 1; }
    if (c1 == 0) {
        // child 1: ls -l > pipe
        dup2(p[1], STDOUT_FILENO);
        close(p[0]); close(p[1]);
        execlp("ls", "ls", "-l", (char *)NULL);
        perror("exec ls"); _exit(127);
    }

    pid_t c2 = fork();
    if (c2 == -1) { perror("fork"); return 1; }
    if (c2 == 0) {
        // child 2: grep <pattern> < pipe
        dup2(p[0], STDIN_FILENO);
        close(p[1]); close(p[0]);
        execlp("grep", "grep", pattern, (char *)NULL);
        perror("exec grep"); _exit(127);
    }

    // parent
    close(p[0]); close(p[1]);
    wait(NULL);
    wait(NULL);
    return 0;
}
