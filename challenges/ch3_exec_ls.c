#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        // Child: replace process with `ls -la`
        execlp("ls", "ls", "-la", (char*)NULL);
        perror("execlp ls");
        _exit(127);
    }

    // Parent: wait for the child to finish
    int status = 0;
    if (waitpid(pid, &status, 0) == -1) { perror("waitpid"); return 1; }

    if (WIFEXITED(status))
        printf("parent: child %d finished with status %d\n", pid, WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("parent: child %d terminated by signal %d\n", pid, WTERMSIG(status));

    return 0;
}