/* https://github.com/cirosantilli/linux-kernel-module-cheat#lkmc-c */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <lkmc.h>

void lkmc_assert_fail(void) {
    exit(1);
}

void lkmc_baremetal_on_exit_callback(int status, void *arg) {
    (void)arg;
    if (status != 0) {
        printf("lkmc_exit_status_%d\n", status);
    }
}

#if defined(__aarch64__)
#define LKMC_SYSREG_READ_WRITE(type, name) \
    type LKMC_CONCAT(LKMC_CONCAT(LKMC_SYSREG_SYMBOL_PREFIX, name), _read(void)) { \
        type name; \
        __asm__ __volatile__("mrs %0, " #name : "=r" (name) : : ); \
        return name; \
    } \
    void LKMC_CONCAT(LKMC_CONCAT(LKMC_SYSREG_SYMBOL_PREFIX, name), _write(type name)) { \
        __asm__ __volatile__("msr " #name ", %0" : : "r" (name) : ); \
    }
LKMC_SYSREG_OPS
#undef LKMC_SYSREG_READ_WRITE
#endif
