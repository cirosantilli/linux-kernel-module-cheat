#include <lkmc.h>

LKMC_PROLOGUE
    /* 1 + 2 == 3 */
    mov $1, %rax
    /* test-gdb-op1 */
    mov $2, %rbx
    add %rax, %rbx
    /* test-gdb-result */
    LKMC_ASSERT_EQ(%rbx, $3)
LKMC_EPILOGUE
