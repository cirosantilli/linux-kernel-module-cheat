/* https://cirosantilli.com/linux-kernel-module-cheat#x86-control-transfer-instructions
 *
 * Unconditional branch, address relative to the current address.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    jmp .Lafter_fail
    LKMC_ASSERT_FAIL
.Lafter_fail:
LKMC_EPILOGUE
