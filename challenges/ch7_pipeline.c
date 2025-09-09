#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pattern>\n", argv[0]);
        return 1;
    }
    const char *pattern = argv[1];

    int p[2];
    if (pipe(p) == -1) { perror("pipe"); return 1; }

    // Child A: run ls, send output to pipe write end
    pid_t ls_pid = fork();
    if (ls_pid < 0) { perror("fork"); return 1; }
    if (ls_pid == 0) {
        if (dup2(p[1], STDOUT_FILENO) == -1) { perror("dup2 ls"); _exit(1); }
        close(p[0]); close(p[1]);
        execlp("ls", "ls", (char *)NULL);      // plain `ls`
        perror("exec ls");
        _exit(127);
    }

    // Child B: run grep <pattern>, read from pipe read end
    pid_t grep_pid = fork();
    if (grep_pid < 0) { perror("fork"); return 1; }
    if (grep_pid == 0) {
        if (dup2(p[0], STDIN_FILENO) == -1) { perror("dup2 grep"); _exit(1); }
        close(p[1]); close(p[0]);
        execlp("grep", "grep", pattern, (char *)NULL);
        perror("exec grep");
        _exit(127);
    }

    // Parent: close both ends and wait for both children
    close(p[0]);
    close(p[1]);
    int status;
    if (waitpid(ls_pid, &status, 0) == -1) perror("waitpid ls");
    if (waitpid(grep_pid, &status, 0) == -1) perror("waitpid grep");
    return 0;
}