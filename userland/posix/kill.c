/* A process that commits suicide by signal. */

#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int sig;
    if (argc <= 1) {
        sig = 1;
    } else {
        sig = strtoull(argv[1], NULL, 0);
    }
    kill(getpid(), sig);
}
