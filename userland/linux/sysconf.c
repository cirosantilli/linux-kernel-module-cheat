/* https://cirosantilli.com/linux-kernel-module-cheat#sysconf */

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    /* Number of processors, not considering affinity:
     * http://stackoverflow.com/questions/2693948/how-do-i-retrieve-the-number-of-processors-on-c-linux */
    printf("_SC_NPROCESSORS_ONLN = %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
    /* CPUs configured by OS during boot. Some may have gone offline, so could be larger than _SC_NPROCESSORS_ONLN.a */
    printf("_SC_NPROCESSORS_CONF = %ld\n", sysconf(_SC_NPROCESSORS_CONF));
    return EXIT_SUCCESS;
}
