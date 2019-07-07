/* https://cirosantilli.com/linux-kernel-module-cheat#arm-adr-instruction */

#include <lkmc.h>

.data
data_label:
    .word 0x1234678
LKMC_PROLOGUE
    /* This is not possible in v7 because the label is in another section.
     * objdump says that this generates a R_AARCH64_ADR_PRE relocation.
     * which looks specific to ADR, and therefore makes it more likely
     * that there was no such relocation in v7.
     *
     * This relocation is particularly important because str does not have a
     * pc-relative mode in ARMv8.
     */
    adr x0, data_label
    ldr x1, =data_label
    LKMC_ASSERT_EQ_REG(x0, x1)
LKMC_EPILOGUE
