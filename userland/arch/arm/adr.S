/* https://cirosantilli.com/linux-kernel-module-cheat#arm-adr-instruction */

#include <lkmc.h>

.data
data_label:
    .word 0x1234678
LKMC_PROLOGUE
    adr r4, .Llabel
    /* objdump tells us that this uses the literal pool,
     * it does not get converted to adr, which is the better
     * alternative here.
     */
    ldr r5, =.Llabel
    adrl r6, .Llabel
.Llabel:
    LKMC_ASSERT_EQ_REG(r4, r5)
    LKMC_ASSERT_EQ_REG(r4, r6)

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
    adr r5, data_label
#endif
LKMC_EPILOGUE
