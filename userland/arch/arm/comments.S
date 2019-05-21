/* https://github.com/cirosantilli/linux-kernel-module-cheat#gnu-gas-assembler-comments */

#include "common.h"
LKMC_ENTRY
    # mycomment
    @ mycomment
    /* # only works at the beginning of the line.
     * Error: garbage following instruction -- `nop #comment'
     */
#if 0
    nop # mycomment
#endif
    nop @ mycomment
LKMC_EXIT
