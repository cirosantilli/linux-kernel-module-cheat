/* # abort
 *
 * Raise a SIGABRT, an ANSI C signal which by default kills the program.
 *
 * ....
 * man abort
 * ....
 *
 * Bibliography:
 *
 * * http://stackoverflow.com/questions/397075/what-is-the-difference-between-exit-and-abort
 * * http://stackoverflow.com/questions/3676221/when-abort-is-preferred-over-exit
 *
 * Differences from exit: does not run regular program teardown:
 *
 * * does not call `atexit` function.
 * * does not call C++ destructors
 *
 * `assert()` exits the program with abort.
 */

#include <stdlib.h>
#include <stdio.h>

void atexit_func() {
    puts("atexit");
}

int main(void) {
    /* Will not get called. */
    atexit(atexit_func);
    abort();
    return EXIT_SUCCESS;
}
