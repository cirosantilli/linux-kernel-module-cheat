/* https://cirosantilli.com/linux-kernel-module-cheat#c
 *
 * Loop and print an integer whenever a period is reached:
 *
 * ....
 * ./infinite_loop [period=100000000 [max=0]]
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
    int max_given;
    if (argc > 1) {
        period = strtoumax(argv[1], NULL, 10);
    } else {
        period = 100000000;
    }
    if (argc > 2) {
        max = strtoumax(argv[2], NULL, 10);
        max_given = 1;
    } else {
        max_given = 0;
    }
    i = 0;
    j = 0;
    while (1) {
        i++;
        if (period != 0 && i % period == 0) {
            printf("%ju\n", j);
            j++;
        }
        if (max_given && i == max)
            break;
    }
}
