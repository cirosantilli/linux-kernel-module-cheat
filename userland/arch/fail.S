/* See what happens on test failure with LKMC_FAIL.
 * https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly
 */

#include "common.h"

LKMC_ENTRY
    LKMC_FAIL
LKMC_EXIT
