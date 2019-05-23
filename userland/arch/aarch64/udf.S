/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-udf-instruction */

#include <lkmc.h>

LKMC_PROLOGUE
    .long 0
#if 0
    udf 0
#endif
LKMC_EPILOGUE
