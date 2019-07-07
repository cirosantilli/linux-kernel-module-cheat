/* https://cirosantilli.com/linux-kernel-module-cheat#arm-b-instruction */

#include <lkmc.h>
LKMC_PROLOGUE
    /* Jump over the fail. 26-bit PC-relative. */
    b .Lok
    LKMC_ASSERT_FAIL
.Lok:
LKMC_EPILOGUE
