/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-b-instruction */

#include <lkmc.h>
LKMC_PROLOGUE
    /* Jump over the fail. 26-bit PC-relative. */
    b ok
    LKMC_ASSERT_FAIL
ok:
LKMC_EPILOGUE
