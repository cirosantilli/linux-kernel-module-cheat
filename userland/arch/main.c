/* This is the main entry point for all .S examples.
 * https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly-c-standard-library
 */

#include <stdio.h>
#include <stdint.h>

#include <lkmc.h>

int asm_main(uint32_t *line);

#define ASSERT_EQ_DEFINE(bits) \
    int assert_eq_ ## bits(uint ## bits ## _t val1, uint ## bits ## _t val2) { \
        if (val1 != val2) { \
            printf("%s failed\n", __func__); \
            printf("val1 0x%" PRIX ## bits "\n", val1); \
            printf("val2 0x%" PRIX ## bits "\n", val2); \
            return 1; \
        } \
        return 0; \
    }

ASSERT_EQ_DEFINE(32)
ASSERT_EQ_DEFINE(64)

int assert_memcmp(const void *s1, const void *s2, size_t n) {
    int ret;
    size_t i;
    uint8_t *s1b, *s2b;
    uint8_t b1, b2;

    ret = 0;
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
            ret = 1;
        }
    }
    return ret;
}

int main(void) {
    uint32_t ret, line;
    ret = asm_main(&line);
    if (ret) {
        printf("error: asm_main returned %d at line %d\n", ret, line);
    }
    return ret;
}
