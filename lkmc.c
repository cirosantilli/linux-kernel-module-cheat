/* https://github.com/cirosantilli/linux-kernel-module-cheat#lkmc-c */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <lkmc.h>

void lkmc_assert(bool condition) {
    if (!condition)
        lkmc_assert_fail();
}

void lkmc_assert_fail() {
    puts("lkmc_test_fail");
    exit(1);
}

bool lkmc_vector_equal(size_t n, double *v1, double *v2, double max_err) {
    double sum = 0.0;
    double diff;
    size_t i;
    for (i = 0; i < n; ++i) {
        diff = v1[i] - v2[i];
        sum += diff * diff;
    }
    if (sqrt(sum)/n > max_err)
        return false;
    return true;
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
