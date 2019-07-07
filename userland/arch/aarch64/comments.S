/* https://cirosantilli.com/linux-kernel-module-cheat#gnu-gas-assembler-comments */

#include <lkmc.h>
LKMC_PROLOGUE
    # mycomment

    /* ARMv8 has // instead of @ as for comments. */
    // mycomment
    nop // mycomment

    /* All these fail. Lol, different than v7, no consistency. */
#if 0
    nop # mycomment
    @ mycomment
    nop @ mycomment
#endif
LKMC_EPILOGUE
