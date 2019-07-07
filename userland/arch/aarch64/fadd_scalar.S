/* https://cirosantilli.com/linux-kernel-module-cheat#armv8-aarch64-fadd-instruction */

#include <lkmc.h>

LKMC_PROLOGUE
    /* 1.5 + 2.5 == 4.0
     * using 64-bit double immediates.
     */
    fmov d0, 1.5
    fmov d1, 2.5
    fadd d2, d0, d1
    fmov d3, 4.0
    /* Unlike VFP vcmp, this stores the status
     * automatically in the main CPSR.
     */
    fcmp d2, d3
    LKMC_ASSERT(beq)

    /* Now with a memory stored value. */
.data
    /* TODO why is this align required, and why only the in the baremetal toolchain?
     * Otherwise at edfbe9f0d7e9cc40cffd1c68c7c7c30a47fda2a8 + 1 was failing with:
     * /path/to/linux-kernel-module-cheat/userland/arch/aarch64/fadd_scalar.S:28:(.text+0x3c): relocation truncated to fit: R_AARCH64_LD_PREL_LO19 against `.data'
     * /path/to/linux-kernel-module-cheat/userland/arch/aarch64/fadd_scalar.S:28: warning: One possible cause of this error is that the symbol is being referenced in the indicated code as if it had a larger alignment than was declared where it was defined.
     */
.align 4
my_double_0:
    .double 1.5
my_double_1:
    .double 2.5
my_double_sum_expect:
    .double 4.0
.text
    ldr d0, my_double_0
    ldr d1, my_double_1
    fadd d2, d0, d1
    ldr d3, my_double_sum_expect
    fcmp d2, d3
    LKMC_ASSERT(beq)

    /* Now in 32-bit. */
    fmov s0, 1.5
    fmov s1, 2.5
    fadd s2, s0, s1
    fmov s3, 4.0
    fcmp s2, s3
    LKMC_ASSERT(beq)

    /* TODO why? What's the point of q then?
     * Error: operand mismatch -- `fmov q0,1.5'
     */
#if 0
    fmov q0, 1.5
#endif

    /* Much like integers, immediates are constrained to
     * fit in 32-byte instructions. TODO exact rules.
     *
     * Assembly here would fail with:
     *
     * Error: invalid floating-point constant at operand 2
     */
#if 0
    fmov d0, 1.23456798
#endif
LKMC_EPILOGUE
