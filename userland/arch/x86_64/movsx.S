/* https://cirosantilli.com/linux-kernel-module-cheat#x86-data-transfer-instructions
 *
 * LIke MOVZX but sign extend.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /* Top bit is 0, extend with zero. */
    mov $0x1234567F, %eax
    movsx %al, %ax
    LKMC_ASSERT_EQ_32(%eax, $0x1234007F)

    /* Top bit is 1: extend with one. */
    mov $0x12345680, %eax
    movsx %al, %ax
    LKMC_ASSERT_EQ_32(%eax, $0x1234FF80)
LKMC_EPILOGUE
