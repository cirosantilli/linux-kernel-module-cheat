/* Loop infinitely. Print an integer whenever a period is reached:
 *
 * ....
 * ./infinite_loop [period]
 * ....
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    uintmax_t i, j, period;
    if (argc > 1) {
        period = strtoumax(argv[1], NULL, 10);
    } else {
        period = 100000000;
    }
    i = 0;
    j = 0;
    while (1) {
        i++;
        if (i % period == 0) {
            printf("%ju\n", j);
            j++;
        }
    }
}
