/* https://cirosantilli.com/linux-kernel-module-cheat#c
 *
 *  Like `sprintf`, but writes at most n bytes, so it is safer,
 *  because it may not be possible or easy to calculate the resulting
 *  size of a formated string.
 *
 *  The size given includes the null terminator. */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
#if __STDC_VERSION__ >= 199901L
    /* Common usage when string fits.
     *
     * Ensures that there will be no out or bounds access on out. */
    {
        int in = 1234;
        char out[1024];
        int snprintf_return;
        snprintf_return = snprintf(out, sizeof(out), "ab%dcd", in);

        /* The usual error checking. */
        if (snprintf_return < 0) {
            perror("snprintf");
            exit(EXIT_FAILURE);
        }
        assert((size_t)snprintf_return < sizeof(out));

        /* Assert because we know the return here.  */
        assert(snprintf_return == 8);

        /* What it actually copied. */
        assert(strcmp(out, "ab1234cd") == 0);
    }

    /* Less common case where string does not fit. Error handling would
     * normally follow in a real program. */
    {
        int in = 1234;
        char out[6];
        /* The return here is the same as before.
         *
         * Because it is >= than the imposed limit of 6, we know that
         * the write failed to fully complete. */
        assert(snprintf(out, sizeof(out), "ab%dcd", in) == 8);
        assert(strcmp(out, "ab123") == 0);
    }
#endif
    return EXIT_SUCCESS;
}

