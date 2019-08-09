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

void lkmc_print_hex_32(uint32_t x) {
    printf("0x%08" PRIX32, x);
}

void lkmc_print_hex_64(uint64_t x) {
    printf("0x%016" PRIX64, x);
}

void lkmc_print_newline() {
    printf("\n");
}

#if defined(__aarch64__)
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
#endif
