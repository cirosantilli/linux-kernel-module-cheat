#include <common.h>

void main(void) {
	int i, j, k;
	i = 1;
    /* test-gdb-op1 */
	j = 2;
    /* test-gdb-op2 */
	k = i + j;
    /* test-gdb-result */
	if (k != 3)
		assert_fail();
}
