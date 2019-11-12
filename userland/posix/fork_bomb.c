/* https://cirosantilli.com/linux-kernel-module-cheat#fork-bomb */

#define _XOPEN_SOURCE 700
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "danger") == 0) {
        while (1) {
            fork();
        }
    }
}
