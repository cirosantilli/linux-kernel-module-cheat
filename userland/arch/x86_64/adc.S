# Add with carry.
#
# edx:eax += ebx:ecx

#include <lkmc.h>

LKMC_PROLOGUE
    movl $0x80000000, %eax
    movl $0x80000000, %ecx
    movl $0, %ebx
    movl $0, %edx
    addl %ecx, %eax
    adcl %ebx, %edx
    LKMC_ASSERT_EQ_32(%eax, $0)
    LKMC_ASSERT_EQ_32(%edx, $1)
LKMC_EPILOGUE
