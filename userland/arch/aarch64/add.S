/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly */

#include <lkmc.h>

LKMC_PROLOGUE
    mov x0, 1
    add x1, x0, 2
    LKMC_ASSERT_EQ(x1, =3)
LKMC_EPILOGUE
