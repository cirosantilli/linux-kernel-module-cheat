/* https://github.com/cirosantilli/linux-kernel-module-cheat#armv8-aarch64-x31-register */

#include "common.h"

LKMC_ENTRY
    /* ERROR: can never use the name x31. */
#if 0
    mov x31, 31
#endif

    /* mov (register) is an alias for ORR, which accepts xzr. */
    mov x19, 1
    mov x19, xzr
    LKMC_ASSERT_EQ(x19, 0)

    /* Same encoding as the mov version. */
    mov x19, 1
    orr x19, xzr, xzr
    LKMC_ASSERT_EQ(x19, 0)

    /* So, orr, which is not an alias, can only take xzr, not sp. */
#if 0
    orr sp, sp, sp
#endif

    /* Zero register discards result if written to. */
    mov x19, 1
    orr xzr, x19, x19
    LKMC_ASSERT_EQ(xzr, 0)

    /* MOV (to/from SP) is an alias for ADD (immediate). */
    mov x19, sp
    mov sp, 1
    /* Alias to add. */
    mov x20, sp
    /* Exact same encoding as above. */
    add x20, sp, 0
    mov sp, x19
    LKMC_ASSERT_EQ(x20, 1)

    /* So, ADD (immediate), which is not an alias, can only take sp, not xzr. */
#if 0
    /* Error: integer register expected in the extended/shifted operand register at operand 3 -- `add xzr,xzr,1' */
    add xzr, xzr, 1
#endif

    /* Note however that ADD (register), unlike ADD (immediate),
     * does not say anything about SP, and so does accept xzr just fine.
     */
    add xzr, xzr, xzr
LKMC_EXIT
