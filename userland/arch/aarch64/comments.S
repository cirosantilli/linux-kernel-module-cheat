/* https://github.com/cirosantilli/arm-assembly-cheat#comments */

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
