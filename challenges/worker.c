#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    printf("worker: argc=%d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d] = '%s'\n", i, argv[i]);
    }

    const char *v = getenv("MYVAR");
    if (v) {
        printf("worker: MYVAR=%s\n", v);
    } else {
        printf("worker: MYVAR is not set\n");
    }

    return 0;
}