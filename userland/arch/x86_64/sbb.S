# Subtract with borrow:
#
#     edx:eax -= ebx:ecx

#include <lkmc.h>

LKMC_PROLOGUE
    movl $0, %eax
    movl $0, %ebx
    movl $0x80000000, %ecx
    movl $1, %edx

    subl %ecx, %eax
    sbbl %ebx, %edx

    LKMC_ASSERT_EQ_32(%eax, $0x80000000)
    LKMC_ASSERT_EQ_32(%edx, $0)

LKMC_EPILOGUE
