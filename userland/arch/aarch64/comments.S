/* https://github.com/cirosantilli/linux-kernel-module-cheat#gnu-gas-assembler-comments */

#include "common.h"
ENTRY
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
EXIT
