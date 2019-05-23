/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly */

#include <lkmc.h>

LKMC_PROLOGUE
    /* Pass */
    LKMC_ASSERT_MEMCMP(var0, var1, =0x10)
    LKMC_ASSERT_MEMCMP(var0, var1, size)
    /* Fail */
    LKMC_ASSERT_MEMCMP(var0, var2, =0x10)
LKMC_EPILOGUE
    var0: .long 0x11111111, 0x22222222, 0x33333333, 0x44444444
    var1: .long 0x11111111, 0x22222222, 0x33333333, 0x44444444
    var2: .long 0x11111111, 0x22222223, 0x23333333, 0x44444444
    size: .quad 0x10
