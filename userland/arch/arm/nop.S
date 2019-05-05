/* https://github.com/cirosantilli/arm-assembly-cheat#nop */

#include "common.h"

ENTRY
    /* Disassembles as:
     *
     * ....
     * nop {0}
     * ....
     *
     * TODO what is the `{0}`?
     */
    nop

    /* Disassembles as:
     *
     * ....
     * nop ; (mov r0, r0)
     * ....
     */
    mov r0, r0

    /* Disassemble as mov. TODO Why not as nop as in `mov r0, r0`?
     * Do they have any effect?
     */
    mov r1, r1
    mov r8, r8

    /* And there are other nops as well? Disassembles as `and`. */
    and r0, r0, r0
EXIT
