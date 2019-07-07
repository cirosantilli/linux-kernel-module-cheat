/* https://cirosantilli.com/linux-kernel-module-cheat#userland-assembly */

#include <lkmc.h>

.data
    myint: .long 0x12345678
LKMC_PROLOGUE
    /* Immediate and register. */
    mov $0, %rax
    mov $1, %rax
    LKMC_ASSERT_EQ(%rax, $1)

    /* Register and register. */
    mov $0, %rax
    mov $1, %rbx
    mov %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $1)

    /* Memory and register. */
    mov myint, %rax
    LKMC_ASSERT_EQ(%rax, $0x12345678)

    /* Memory and immediate. */
    movl $0x9ABCDEF0, myint
    LKMC_ASSERT_EQ(myint, $0x9ABCDEF0)

    /* Memory via pointer to address. */
    /* eax = &myint */
    mov $myint, %rax
    movl $0x11112222, (%rax)
    LKMC_ASSERT_EQ(myint, $0x11112222)

    /* Possible to move on itself, seems like a NOP and way to clear 32 high bits in x86-64:
     * http://stackoverflow.com/questions/11910501/why-did-gcc-generate-mov-eax-eax-and-what-does-it-mean
     */
    mov %rax, %rax
LKMC_EPILOGUE
