/* https://cirosantilli.com/linux-kernel-module-cheat#memory-size */

#define _GNU_SOURCE
#include <stdio.h>
#include <sys/sysinfo.h>
#include <unistd.h>

int main(void) {
    /* PAGESIZE is POSIX: http://pubs.opengroup.org/onlinepubs/9699919799/
     * but PHYS_PAGES and AVPHYS_PAGES are glibc extensions. I bet those are
     * parsed from /proc/meminfo. */
    printf(
        "sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE) = 0x%lX\n",
        sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE)
    );
    printf(
        "sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGESIZE) = 0x%lX\n",
        sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGESIZE)
    );

    /* glibc extensions. man says they are parsed from /proc/meminfo. */
    printf(
        "get_phys_pages() * sysconf(_SC_PAGESIZE) = 0x%lX\n",
        get_phys_pages() * sysconf(_SC_PAGESIZE)
    );
    printf(
        "get_avphys_pages() * sysconf(_SC_PAGESIZE) = 0x%lX\n",
        get_avphys_pages() * sysconf(_SC_PAGESIZE)
    );
}
