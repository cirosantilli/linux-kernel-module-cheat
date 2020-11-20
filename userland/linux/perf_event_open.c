/* https://cirosantilli.com/linux-kernel-module-cheat#perf-event-open
 * Adapted from `man perf_event_open` in manpages 5.05-1. */

#define _GNU_SOURCE
#include <asm/unistd.h>
#include <linux/perf_event.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#define LKMC_M5OPS_ENABLE 1
#include <lkmc.h>

static long perf_event_open(struct perf_event_attr *hw_event,
    uint32_t type, uint64_t config, pid_t pid,
    int cpu, int group_fd, unsigned long flags
) {
    int ret;

    memset(hw_event, 0, sizeof(struct perf_event_attr));
    hw_event->type = type;
    hw_event->size = sizeof(struct perf_event_attr);
    hw_event->config = config;
    hw_event->disabled = 1;
    hw_event->exclude_kernel = 1;
    /* Don't count hypervisor events. */
    hw_event->exclude_hv = 1;
    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                    group_fd, flags);
    return ret;
}

typedef struct {
    char *name;
    uint32_t type;
    uint64_t config;
} Desc;

#define DESC(type,config) {#config, type, config}

int
main(int argc, char **argv) {
    size_t i;
    int gem5;
    long long count;
    uint64_t n;
    Desc descs[] = {
        /* ARMV8_PMUV3_PERFCTR_PC_WRITE_RETIRED = 0x0C */
        DESC(PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS),

        /* ARMV8_PMUV3_PERFCTR_BR_MIS_PRED = 0x10 */
        DESC(PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES),

        /* ARMV8_PMUV3_PERFCTR_CPU_CYCLES = 0x11 */
        DESC(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES),
        DESC(PERF_TYPE_HARDWARE, PERF_COUNT_HW_REF_CPU_CYCLES),

        /* ARMV8_PMUV3_PERFCTR_INST_RETIRED = 0x08 */
        DESC(PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS),
        DESC(PERF_TYPE_HW_CACHE, PERF_COUNT_HW_CACHE_L1D | PERF_COUNT_HW_CACHE_OP_READ << 8 | PERF_COUNT_HW_CACHE_RESULT_MISS << 16),
    };
    struct perf_event_attr pes[LKMC_ARRAY_SIZE(descs)];
    int fds[LKMC_ARRAY_SIZE(descs)];

    if (argc > 1) {
        n = strtoll(argv[1], NULL, 0);
    } else {
        n = 10000;
    }
    if (argc > 2) {
        gem5 = argv[2][0] == '1';
    } else {
        gem5 = 0;
    }

    for (i = 0; i < LKMC_ARRAY_SIZE(descs); i++) {
        fds[i] = perf_event_open(&pes[i],
            descs[i].type, descs[i].config, 0, -1, -1, 0);
        if (fds[i] == -1) {
            fprintf(
                stderr, "perf_event_open error name=%s type=%zx config=%zx\n",
                descs[i].name, (uintmax_t)pes[i].type, (uintmax_t)pes[i].config
            );
            /*exit(EXIT_FAILURE);*/
        }
    }

    /* Start the counts. */
    for (i = 0; i < LKMC_ARRAY_SIZE(descs); i++)
        ioctl(fds[i], PERF_EVENT_IOC_RESET, 0);
    for (i = 0; i < LKMC_ARRAY_SIZE(descs); i++)
        ioctl(fds[i], PERF_EVENT_IOC_ENABLE, 0);
    if (gem5) {
        /* Cross check with gem5 stats to see if things make sense.. */
        LKMC_M5OPS_RESETSTATS;
    }

    /* Mesure this function. */
    lkmc_busy_loop(n, 1);

    /* Stop the count. */
    if (gem5) { LKMC_M5OPS_DUMPSTATS; }
    for (i = 0; i < LKMC_ARRAY_SIZE(descs); i++)
        ioctl(fds[i], PERF_EVENT_IOC_DISABLE, 0);

    /* Print results. */
    for (i = 0; i < LKMC_ARRAY_SIZE(descs); i++) {
        read(fds[i], &count, sizeof(long long));
        printf("%s %lld\n", descs[i].name, count);
    }

    for (i = 0; i < LKMC_ARRAY_SIZE(descs); i++)
        close(fds[i]);
}
