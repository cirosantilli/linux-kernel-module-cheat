/* https://cirosantilli.com/linux-kernel-module-cheat#lkmc-c */

#include <math.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

#include <lkmc.h>

#define LKMC_ASSERT_EQ_DEFINE(bits) \
    LKMC_ASSERT_EQ_DECLARE(bits) \
    { \
        if (val1 != val2) { \
            printf("%s failed\n", __func__); \
            printf("val1 0x%" PRIX ## bits "\n", val1); \
            printf("val2 0x%" PRIX ## bits "\n", val2); \
            lkmc_assert_fail(line); \
        } \
    }
LKMC_ASSERT_EQ_DEFINE(32)
LKMC_ASSERT_EQ_DEFINE(64)
#undef ASSERT_EQ_DEFINE

void lkmc_assert_fail(uint32_t line) {
    printf("error: assertion failed at line: %" PRIu32 "\n", line);
    fflush(stdout);
    abort();
}

void lkmc_assert_memcmp(
    const void *s1,
    const void *s2,
    size_t n,
    uint32_t line
) {
    size_t i;
    uint8_t *s1b, *s2b;
    uint8_t b1, b2;

    s1b = (uint8_t *)s1;
    s2b = (uint8_t *)s2;
    for (i = 0; i < n; ++i) {
        b1 = s1b[i];
        b2 = s2b[i];
        if (b1 != b2) {
            printf(
                "%s failed: "
                "byte1, byte2, index: "
                "0x%02" PRIX8 " 0x%02" PRIX8 " 0x%zX\n",
                __func__,
                b1,
                b2,
                i
            );
            lkmc_assert_fail(line);
        }
    }
}

void __attribute__ ((noinline)) lkmc_busy_loop(
    unsigned long long max,
    unsigned long long max2
) {
    for (unsigned long long i = 0; i < max2; i++) {
        for (unsigned long long j = 0; j < max; j++) {
            __asm__ __volatile__ ("" : "+g" (i), "+g" (j) : :);
        }
    }
}

void lkmc_print_hex_32(uint32_t x) {
    printf("0x%08" PRIX32, x);
}

void lkmc_print_hex_64(uint64_t x) {
    printf("0x%016" PRIX64, x);
}

void lkmc_print_newline() {
    printf("\n");
}

#if defined(__arm__)

void lkmc_arm_psci_cpu_on(
    uint32_t target_cpu,
    uint32_t entry_point_address,
    uint32_t context_id
) {
    register int r0 __asm__ ("r0") = 0x84000003;
    register int r1 __asm__ ("r1") = target_cpu;
    register int r2 __asm__ ("r2") = entry_point_address;
    register int r3 __asm__ ("r3") = context_id;
    __asm__ __volatile__(
        "hvc 0\n"
        :
        : "r" (r0),
          "r" (r1),
          "r" (r2),
          "r" (r3)
        :
    );
}

#elif defined(__aarch64__)

#define LKMC_SYSREG_READ_WRITE(nbits, name) \
    LKMC_CONCAT(LKMC_CONCAT(uint, nbits), _t) LKMC_CONCAT(LKMC_CONCAT(LKMC_SYSREG_SYMBOL_PREFIX, read_), name)(void) { \
        LKMC_CONCAT(LKMC_CONCAT(uint, nbits), _t) name; \
        __asm__ __volatile__("mrs %0, " #name : "=r" (name) : : ); \
        return name; \
    } \
    void LKMC_CONCAT(LKMC_CONCAT(LKMC_SYSREG_SYMBOL_PREFIX, write_), name)(LKMC_CONCAT(LKMC_CONCAT(uint, nbits), _t) name) { \
        __asm__ __volatile__("msr " #name ", %0" : : "r" (name) : ); \
    } \
    void LKMC_CONCAT(LKMC_CONCAT(LKMC_SYSREG_SYMBOL_PREFIX, print_), name)(void) { \
        printf(#name " 0x%" PRIX ## nbits "\n", LKMC_CONCAT(LKMC_CONCAT(LKMC_SYSREG_SYMBOL_PREFIX, read_), name)()); \
    }
LKMC_SYSREG_OPS
#undef LKMC_SYSREG_READ_WRITE

uint64_t lkmc_aarch64_cpu_id() {
    /* TODO: cores beyond 4th?
     * Mnemonic: Main Processor ID Register
     */
    return lkmc_sysreg_read_mpidr_el1() & 3;
}

void lkmc_aarch64_psci_cpu_on(
    uint64_t target_cpu,
    uint64_t entry_point_address,
    uint64_t context_id
) {
    register int w0 __asm__ ("w0") = 0xc4000003;
    register int x1 __asm__ ("x1") = target_cpu;
    register int x2 __asm__ ("x2") = entry_point_address;
    register int x3 __asm__ ("x3") = context_id;
    __asm__ __volatile__(
        "hvc 0\n"
        :
        : "r" (w0),
          "r" (x1),
          "r" (x2),
          "r" (x3)
        :
    );
}
#endif
