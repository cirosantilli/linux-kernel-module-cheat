/* https://cirosantilli.com/linux-kernel-module-cheat#c
 *
 * Multidimentional arrays are generally a bad idea that confuses
 * everyone, use single dimentional arrays + indexing if possible.
 * But here goes nothing.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    /* Initialized in the code. */
    {
        /* We can skip the first dimension as it is inferred. */
        int is[][3] = {
            {1, 2, 3,},
            {4, 5, 5,},
        };
        assert(is[0][0] == 1);
        assert(is[0][1] == 2);
        assert(is[1][0] == 4);

        /* We can get the total sizes of either the entire array,
         * or of just on row. */
        assert(sizeof(is) == 6 * sizeof(is[0][0]));
        assert(sizeof(is[0]) == 3 * sizeof(is[0][0]));

        /* Multi dimentional arrays are contiguous and row major. */
        assert(&is[0][1] - &is[0][0] == 1);
        assert(&is[1][0] - &is[0][0] == 3);
    }
    return EXIT_SUCCESS;
}
