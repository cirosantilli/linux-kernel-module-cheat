/* https://cirosantilli.com/linux-kernel-module-cheat#sysconf */

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SYSCONF(x) printf("_SC_%-23s = %ld\n", #x, sysconf(_SC_ ## x))

int main(void) {
    /* Number of processors, not considering affinity:
     * http://stackoverflow.com/questions/2693948/how-do-i-retrieve-the-number-of-processors-on-c-linux */
    SYSCONF(NPROCESSORS_ONLN);
    /* CPUs configured by OS during boot. Some may have gone offline, so could be larger than _SC_NPROCESSORS_ONLN.a */
    SYSCONF(NPROCESSORS_CONF);

    /* https://cirosantilli.com/linux-kernel-module-cheat#gem5-cache-size */
    SYSCONF(LEVEL1_ICACHE_SIZE);
    SYSCONF(LEVEL1_ICACHE_ASSOC);
    SYSCONF(LEVEL1_ICACHE_LINESIZE);
    SYSCONF(LEVEL1_DCACHE_SIZE);
    SYSCONF(LEVEL1_DCACHE_ASSOC);
    SYSCONF(LEVEL1_DCACHE_LINESIZE);
    SYSCONF(LEVEL2_CACHE_SIZE);
    SYSCONF(LEVEL2_CACHE_ASSOC);
    SYSCONF(LEVEL2_CACHE_LINESIZE);
    SYSCONF(LEVEL3_CACHE_SIZE);
    SYSCONF(LEVEL3_CACHE_ASSOC);
    SYSCONF(LEVEL3_CACHE_LINESIZE);
    SYSCONF(LEVEL4_CACHE_SIZE);
    SYSCONF(LEVEL4_CACHE_ASSOC);
    SYSCONF(LEVEL4_CACHE_LINESIZE);

    return EXIT_SUCCESS;
}
