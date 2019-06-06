/* Increment: i++. */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $2, %eax
    inc %eax
    LKMC_ASSERT_EQ_32(%eax, $3)
LKMC_EPILOGUE
