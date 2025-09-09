#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    for (int s = 1; s <= 3; s++) {
        if (fork() == 0) {          // child
            sleep(s);
            _exit(s);               // exit code == sleep seconds
        }
    }

    int remaining = 3, status;
    while (remaining > 0) {
        pid_t w = waitpid(-1, &status, WNOHANG);
        if (w > 0) {
            if (WIFEXITED(status))
                printf("parent: child %d finished (status %d)\n", (int)w, WEXITSTATUS(status));
            remaining--;
        } else if (w == 0) {
            usleep(100000);         // 100 ms; avoid busy-waiting
        } else {
            break;                  // no children left or error
        }
    }
    return 0;
}