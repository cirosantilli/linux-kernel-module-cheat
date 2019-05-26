#include <lkmc.h>

LKMC_PROLOGUE
    /* 1 + 2 == 3 */
    mov x0, 1
    /* test-gdb-op1 */
    add x1, x0, 2
    /* test-gdb-result */
    LKMC_ASSERT_EQ(x1, =3)
LKMC_EPILOGUE
