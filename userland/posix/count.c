/* Count to infinity with 1 second sleep between each increment.
 * Sample application: https://cirosantilli.com/linux-kernel-module-cheat#gdb-step-debug-userland-custom-init
 */

#define _XOPEN_SOURCE 700
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    unsigned long i = 0;
    while (1) {
        printf("%lu\n", i);
        i++;
        sleep(1);
    }
}
