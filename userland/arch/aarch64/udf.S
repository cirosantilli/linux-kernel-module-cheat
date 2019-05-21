/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-udf-instruction */

#include "common.h"

LKMC_ENTRY
    .long 0
#if 0
    udf 0
#endif
LKMC_EXIT
