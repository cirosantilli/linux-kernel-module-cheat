#include <lkmc.h>

int main(void) {
    int i, j, k;
    i = 1;
    /* test-gdb-op1 */
    j = 2;
    /* test-gdb-op2 */
    k = i + j;
    /* test-gdb-result */
    if (k != 3)
        lkmc_assert_fail();
}
