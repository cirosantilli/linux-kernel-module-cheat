/* https://cirosantilli.com/linux-kernel-module-cheat#malloc-maximum-size
 *
 * mmap memory, then write something to each page to ensure it is not just virtual.
 * We want to meet the OOM.
 *
 * ./prog [nbytes [print_interval]]
 */

#define _GNU_SOURCE
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct {
    unsigned long size,resident,share,text,lib,data,dt;
} ProcStatm;

/* https://stackoverflow.com/questions/1558402/memory-usage-of-current-process-in-c/7212248#7212248 */
void ProcStat_init(ProcStatm *result) {
    const char* statm_path = "/proc/self/statm";
    FILE *f = fopen(statm_path, "r");
    if(!f) {
        perror(statm_path);
        abort();
    }
    if(7 != fscanf(
        f,
        "%lu %lu %lu %lu %lu %lu %lu",
        &(result->size),
        &(result->resident),
        &(result->share),
        &(result->text),
        &(result->lib),
        &(result->data),
        &(result->dt)
    )) {
        perror(statm_path);
        abort();
    }
    fclose(f);
}

int main(int argc, char **argv) {
    ProcStatm proc_statm;
    char *base, *p;
    char system_cmd[1024];
    long page_size;
    size_t i, nbytes, print_interval, bytes_since_last_print;
    int snprintf_return;

    /* Decide how many ints to allocate. */
    if (argc < 2) {
        nbytes = 0x10000;
    } else {
        nbytes = strtoull(argv[1], NULL, 0);
    }
    if (argc < 3) {
        print_interval = 0x1000;
    } else {
        print_interval = strtoull(argv[2], NULL, 0);
    }
    page_size = sysconf(_SC_PAGESIZE);

    /* Allocate the memory. */
    base = mmap(
        NULL,
        nbytes,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0
    );
    if (base == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    /* Write to all the allocated pages. */
    i = 0;
    p = base;
    bytes_since_last_print = 0;
    /* Produce the ps command that lists only our VSZ and RSS. */
    snprintf_return = snprintf(
        system_cmd,
        sizeof(system_cmd),
        "ps -o pid,vsz,rss | awk '{if (NR == 1 || $1 == \"%ju\") print}'",
        (uintmax_t)getpid()
    );
    assert(snprintf_return >= 0);
    assert((size_t)snprintf_return < sizeof(system_cmd));
    bytes_since_last_print = print_interval;
    do {
        /* Modify a byte in the page. */
        *p = i;
        p += page_size;
        bytes_since_last_print += page_size;
        /* Print process memory usage every print_interval bytes.
         * We count memory using a few techniques from:
         * https://stackoverflow.com/questions/1558402/memory-usage-of-current-process-in-c */
        if (bytes_since_last_print > print_interval) {
            bytes_since_last_print -= print_interval;
            printf("extra_memory_committed %lu KiB\n", (i * page_size) / 1024);
            ProcStat_init(&proc_statm);
            /* Check /proc/self/statm */
            printf(
                "/proc/self/statm size resident %lu %lu KiB\n",
                (proc_statm.size * page_size) / 1024,
                (proc_statm.resident * page_size) / 1024
            );
            /* Check ps. */
            puts(system_cmd);
            system(system_cmd);
            puts("");
        }
        i++;
    } while (p < base + nbytes);

    /* Cleanup. */
    munmap(base, nbytes);
    return EXIT_SUCCESS;
}
