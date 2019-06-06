/* Subtraction. */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $3, %rax
    sub $2, %rax
    LKMC_ASSERT_EQ(%rax, $1)
LKMC_EPILOGUE
