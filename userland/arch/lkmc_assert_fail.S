/* See what happens on test failure with LKMC_ASSERT_FAIL.
 * https://cirosantilli.com/linux-kernel-module-cheat#userland-assembly
 */

#include <lkmc.h>

LKMC_PROLOGUE
    LKMC_ASSERT_FAIL
LKMC_EPILOGUE
