/* https://cirosantilli.com/linux-kernel-module-cheat#x86-enter-and-leave-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    /* Save values of interest before enter. */
    mov %rbp, %r12
    mov %rsp, %r13
    enter $16, $0
    mov %rsp, %r14

    /* Restore stack so that we can do our assertions. */
    add $16, %rsp
    leave

    /* ENTER pushed the stack down 24 bytes:
     *
     * * 8 due to `push %rbp` that `enter` does automatically
     * * 16 due to `enter $16`
     */
    sub %r14, %r13
    LKMC_ASSERT_EQ(%r13, $24)
LKMC_EPILOGUE
