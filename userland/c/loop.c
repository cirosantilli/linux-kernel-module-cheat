/* https://cirosantilli.com/linux-kernel-module-cheat#c
 *
 * Loop and print an integer whenever a period is reached:
 *
 * ....
 * ./loop.out [max=10 [period=1]]
 * ....
 *
 * * period: period for printing integers to stdout
 *           0 means disable printing.
 * * max:    Stop counting when max is reached.
 *           0 means count to infinity.
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    uintmax_t i, j, period, max;
    if (argc > 1) {
        max = strtoumax(argv[1], NULL, 10);
    } else {
        max = 10;
    }
    if (argc > 2) {
        period = strtoumax(argv[2], NULL, 10);
    } else {
        period = 1;
    }
    i = 0;
    j = 0;
    while (1) {
        if (period != 0 && i % period == 0) {
            printf("%ju\n", j);
            j++;
        }
        i++;
        if (i == max)
            break;
    }
}
