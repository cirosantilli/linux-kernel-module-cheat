/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-cbz-instruction */

#include <lkmc.h>

LKMC_PROLOGUE

    /* Branch. */
    mov x0, 0x0
    cbz x0, ok
    LKMC_ASSERT_FAIL
ok:

    /* Don't branch. */
    mov x0, 0x1
    cbz x0, ko

LKMC_EPILOGUE
ko:
    LKMC_ASSERT_FAIL
