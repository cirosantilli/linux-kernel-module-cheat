/* https://github.com/cirosantilli/arm-assembly-cheat#adr */

#include "common.h"

.data
data_label:
    .word 0x1234678
ENTRY
    adr r0, label
    /* objdump tells us that this uses the literal pool,
     * it does not get converted to adr, which is the better
     * alternative here.
     */
    adr r1, label
    adrl r2, label
label:
    ASSERT_EQ_REG(r0, r1)
    ASSERT_EQ_REG(r0, r2)

#if 0
    /* Error: symbol .data is in a different section.
     *
     * It works however in ARMv8.
     * I think this means that there is no relocation type
     * that takes care of this encoding in ARMv8, but there
     * is one in ARMv8.
     *
     * If you have no idea what I'm talking about, read this:
     * https://stackoverflow.com/questions/3322911/what-do-linkers-do/33690144#33690144
     */
    adr r1, data_label
#endif
EXIT
