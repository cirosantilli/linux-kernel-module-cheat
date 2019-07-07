/* https://cirosantilli.com/linux-kernel-module-cheat#x86-shift-and-rotate-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $0x81, %r12

    /* axl = 03h, CF = 1 */
    rol $1, %r12b
    LKMC_ASSERT(jc)
    LKMC_ASSERT_EQ(%r12, $3)

    /* axl = 04h, CF = 0 */
    rol $1, %r12b
    LKMC_ASSERT(jnc)
    LKMC_ASSERT_EQ(%r12, $6)

    /* axl = 03h, CF = 0 */
    ror $2, %r12b
    LKMC_ASSERT(jc)
    LKMC_ASSERT_EQ(%r12, $0x81)

    /* axl = 81h, CF = 1 */
    ror $1, %r12b
    LKMC_ASSERT(jc)
    LKMC_ASSERT_EQ(%r12, $0x0C0)
LKMC_EPILOGUE
