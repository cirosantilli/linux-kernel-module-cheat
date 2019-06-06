/* Decrement: i--. */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $3, %rax
    dec %rax
    LKMC_ASSERT_EQ_32(%rax, $2)
LKMC_EPILOGUE
