/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-addressing-modes */

#include <lkmc.h>

LKMC_PROLOGUE

    /* First we play around with lea which is easier to assert. */

    /* Full form with immediates:
     *
     * rbx + rcx * 2 + 4 =
     * 3   + 4   * 2 + 4 =
     * 3   +       8 + 4 =
     * 3   +       8 + 4 =
     * 15
     */
    mov $0, %rax
    mov $3, %rbx
    mov $4, %rcx
    /* GAS 2.24 Warning: segment override on `lea' is ineffectual. */
    /*lea %ds:4(%rbx, %rcx, 2), %rax*/
    lea 4(%rbx, %rcx, 2), %rax
    LKMC_ASSERT_EQ(%rax, $15)

    /* Omit the mulitplicator d.
     * a(b,c) == a(b,c,1)
     */
    mov $0, %rax
    mov $3, %rbx
    mov $4, %rcx
    lea 2(%rbx, %rcx), %rax
    LKMC_ASSERT_EQ(%rax, $9)

    /* Omit c and d. */
    mov $0, %rax
    mov $1, %rbx
    lea 2(%rbx), %rax
    LKMC_ASSERT_EQ(%rax, $3)

    /* Register only address. We can omit commas. */
    mov $0, %rax
    mov $1, %rbx
    lea (%rbx), %rax
    LKMC_ASSERT_EQ(%rax, $1)

    /* TODO What is this syntax for? Compare to the next example. */
    mov $0, %rax
    lea 2(,1), %rax
    LKMC_ASSERT_EQ(%rax, $2)

    mov $0, %rax
    lea 2, %rax
    LKMC_ASSERT_EQ(%rax, $2)

    /* TODO What is this syntax for? Compare to the previous example. */
    mov $0, %rax
    lea (2), %rax
    LKMC_ASSERT_EQ(%rax, $2)

    mov $0, %rax
    mov $3, %rbx
    lea 2(,%rbx,4), %rax
    LKMC_ASSERT_EQ(%rax, $14)

    /* Expressions like (1 + 1) or more commonly (label + 1)
     * can be used like anywhere else: the assembler / linker resolve
     * them for us.
     */
    mov $1, %rax
    lea (1 + 1)(%rax), %rax
    LKMC_ASSERT_EQ(%rax, $3)

    /* Now some examples with the label and actual memory movs just for concreteness. */
.data
   myint: .long 0x12345678
.text

    /* Pointer dereference: To get the actual address instead of the data, use `$`: */
    mov $myint, %rbx
    mov (%rbx), %eax
    LKMC_ASSERT_EQ_32(%eax, myint)

    /* Regular memory IO is just a subcase of the full addressing mode syntax! */
    mov $0, %rax
    movl $0x9ABCDEF0, myint
    mov myint, %rax
    LKMC_ASSERT_EQ_32(%eax, $0x9ABCDEF0)

    /* Other instructions like add can also use the addressing. */
    movl $1, myint
    mov $myint, %rbx
    addl $2, (%rbx)
    LKMC_ASSERT_EQ_32(myint, $3)

LKMC_EPILOGUE
