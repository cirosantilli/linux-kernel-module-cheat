#include <lkmc.h>

LKMC_PROLOGUE
    /* 1 + 2 == 3 */
    mov r0, 1
    /* test-gdb-op1 */
    add r1, r0, 2
    /* test-gdb-result */
    LKMC_ASSERT_EQ(r1, =3)
LKMC_EPILOGUE
