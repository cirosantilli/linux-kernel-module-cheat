/* https://github.com/cirosantilli/arm-assembly-cheat#x31 */

#include "common.h"

ENTRY
    /* ERROR: can never use the name x31. */
#if 0
    mov x31, 31
#endif

    /* mov (register) is an alias for ORR, which accepts xzr. */
    mov x0, 1
    mov x0, xzr
    ASSERT_EQ(x0, 0)

    /* Same encoding as the mov version. */
    mov x0, 1
    orr x0, xzr, xzr
    ASSERT_EQ(x0, 0)

    /* So, orr, which is not an alias, can only take xzr, not sp. */
#if 0
    orr sp, sp, sp
#endif

    /* Zero register discards result if written to. */
    mov x0, 1
    orr xzr, x0, x0
    ASSERT_EQ(xzr, 0)

    /* MOV (to/from SP) is an alias for ADD (immediate). */
    mov x0, sp
    mov sp, 1
    /* Alias to add. */
    mov x1, sp
    /* Exact same encoding as above. */
    add x1, sp, 0
    ASSERT_EQ(x1, 1)
    mov sp, x0

    /* So, ADD (immediate), which is not an alias, can only take sp, not xzr. */
#if 0
    /* Error: integer register expected in the extended/shifted operand register at operand 3 -- `add xzr,xzr,1' */
    add xzr, xzr, 1
#endif

    /* Note however that ADD (register), unlike ADD (immediate),
     * does not say anything about SP, and so does accept xzr just fine.
     */
    add xzr, xzr, xzr
EXIT
