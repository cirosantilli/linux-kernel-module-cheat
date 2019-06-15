/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-control-transfer-instructions
 *
 * Unconditional branch, address relative to the current address.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    jmp after_fail
    LKMC_ASSERT_FAIL
after_fail:
LKMC_EPILOGUE
