/* Count up to a given number 1 second sleep between each increment.
 *
 * https://cirosantilli.com/linux-kernel-module-cheat#unistd-h
 *
 * We need a separate program for this from count.c because count.c
 * is also usable as an init process, where we can't control the CLI
 * arguments very well.
 */

#define _XOPEN_SOURCE 700
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    unsigned long i, max, sleep_time;
    if (argc > 1) {
        max = strtoll(argv[1], NULL, 0);
    } else {
        max = 1;
    }
    if (argc > 2) {
        sleep_time = strtoll(argv[2], NULL, 0);
    } else {
        sleep_time = 2;
    }
    i = 0;
    while (i < max) {
        printf("%lu\n", i);
        i++;
        sleep(sleep_time);
    }
}
