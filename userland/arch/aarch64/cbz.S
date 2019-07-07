/* https://cirosantilli.com/linux-kernel-module-cheat#arm-cbz-instruction */

#include <lkmc.h>

LKMC_PROLOGUE

    /* Branch. */
    mov x0, 0x0
    cbz x0, .Lok
    LKMC_ASSERT_FAIL
.Lok:

    /* Don't branch. */
    mov x0, 0x1
    cbz x0, .Lko
LKMC_EPILOGUE
.Lko:
    LKMC_ASSERT_FAIL
