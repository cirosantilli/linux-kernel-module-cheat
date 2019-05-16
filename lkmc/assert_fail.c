/* Let's see what happens when an assert fails.
 *
 * Outcome on Ubuntu 19.04 shows the failure line:
 *
 *     assert_fail.out: /path/to/linux-kernel-module-cheat/userland/c/assert_fail.c:15: main: Assertion `0' failed.
 *
 * and exit status 134 == 128 + 6, which corresponds to SIGABORT (6).
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    assert(0);
    puts("here");
    return EXIT_SUCCESS;
}
