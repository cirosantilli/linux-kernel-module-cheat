/* See what happens on test failure.
 * https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly
 */

#include "common.h"

ENTRY
#if 0
    /* Uncomment this to see it fail. */
    FAIL
#endif
EXIT
