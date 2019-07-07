/* https://cirosantilli.com/linux-kernel-module-cheat#gnu-gas-assembler-comments */

#include <lkmc.h>
LKMC_PROLOGUE
    # mycomment
    @ mycomment
    /* # only works at the beginning of the line.
     * Error: garbage following instruction -- `nop #comment'
     */
#if 0
    nop # mycomment
#endif
    nop @ mycomment
LKMC_EPILOGUE
