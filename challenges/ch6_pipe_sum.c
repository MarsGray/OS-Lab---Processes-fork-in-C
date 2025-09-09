#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int p[2];
    if (pipe(p) == -1) { perror("pipe"); return 1; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        // Child: read from pipe, sum, print result
        close(p[1]);                       // close write end
        FILE *in = fdopen(p[0], "r");
        if (!in) { perror("fdopen"); _exit(1); }

        int x, sum = 0;
        while (fscanf(in, "%d", &x) == 1) sum += x;

        printf("Sum = %d\n", sum);         // Acceptance: should print "Sum = 55"
        fclose(in);
        _exit(0);
    } else {
        // Parent: write numbers 1..10 to pipe, then wait
        close(p[0]);                       // close read end
        FILE *out = fdopen(p[1], "w");
        if (!out) { perror("fdopen"); return 1; }

        for (int i = 1; i <= 10; i++) fprintf(out, "%d ", i);
        fclose(out);                       // flush + close -> EOF for child

        int status;
        waitpid(pid, &status, 0);
        return 0;
    }
}