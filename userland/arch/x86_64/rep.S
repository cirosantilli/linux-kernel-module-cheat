/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-rep-prefix */

#include <lkmc.h>

.bss
    src: .skip 16
    dst: .skip 16
LKMC_PROLOGUE

    /* memset: REP STOSQ */
    cld
    lea dst(%rip), %rdi
    /* 2 elements. */
    mov $2, %rcx
    /* Set every element to 42. */
    mov $0x2A, %rax
    rep stosq
    /* RCX was decremented down to zero. */
    LKMC_ASSERT_EQ(%rcx, $0)
    /* And the memory was set. */
    LKMC_ASSERT_EQ(dst + 0, $0x2A)
    LKMC_ASSERT_EQ(dst + 8, $0x2A)

    /* memcpy: REP MOVSQ */
    cld
    movq $2, src + 0
    movq $3, src + 8
    lea src(%rip), %rsi
    lea dst(%rip), %rdi
    mov $2, %rcx
    rep movsq
    LKMC_ASSERT_EQ(dst + 0, $2)
    LKMC_ASSERT_EQ(dst + 8, $3)

    /* memcmp: REPZ CMPSQ */

        /* Setup src. */
        movl $2, src + 0x0
        movl $3, src + 0x4
        movl $4, src + 0x8
        movl $5, src + 0xA

        /* Equal. */
        movl $2, dst + 0x0
        movl $3, dst + 0x4
        movl $4, dst + 0x8
        movl $5, dst + 0xA
        cld
        mov $src, %rsi
        mov $dst, %rdi
        mov $4, %rcx
        repz cmpsl
        mov %rcx, %r12
        /* Last flag was equal. */
        LKMC_ASSERT(jz)
        /* RCX was decreased all the way to zero. */
        LKMC_ASSERT_EQ(%r12, $0)

        /* Different. */
        movl $2, dst + 0x0
        movl $3, dst + 0x4
        movl $2, dst + 0x8
        movl $5, dst + 0xA
        mov $src, %rsi
        mov $dst, %rdi
        mov $4, %rcx
        repz cmpsl
        mov %rcx, %r12
        LKMC_ASSERT(jnz)
        /* We stopped half-way with 1 comparision missing. */
        LKMC_ASSERT_EQ(%r12, $1)

LKMC_EPILOGUE
