/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-binary-arithmetic-instructions
 *
 * Unsigned integer division, interface similar to MUL:
 *
 * ....
 * rax = rdx:rax / SRC
 * rdx = rdx:rax % SRC
 * ....
 *
 * DIV can be used to calculate modulus, but GCC does not use it becaues it is slow, 
 * and choses alternative techniques instead
 * http://stackoverflow.com/questions/4361979/how-does-the-gcc-implementation-of-module-work-and-why-does-it-not-use-the
 */

#include <lkmc.h>

LKMC_PROLOGUE

    /* 64-bit hello world:
     *
     * 5 / 2 = 2 with leftover of 1.
     */
    mov $0, %rdx
    mov $5, %rax
    mov $2, %rbx
    div %rbx
    mov %rax, %r12
    mov %rdx, %r13
    LKMC_ASSERT_EQ(%r12, $2)
    LKMC_ASSERT_EQ(%r13, $1)

    /* Now with a simple carry. */
    mov $1, %rdx
    mov $2, %rax
    mov $2, %rbx
    div %rbx
    mov %rax, %r12
    mov %rdx, %r13
    LKMC_ASSERT_EQ(%r12, $0x8000000000000001)
    LKMC_ASSERT_EQ(%r13, $0)

    /* TODO SIGFPE example does not fit into rax. */
    mov $2, %rdx
    mov $0, %rax
    mov $2, %rbx
    div %rbx

#if 0
    /* 32 bit */
    mov $1, %eax
    mov $1, %edx
    mov $2, %ecx
    div %ecx
    LKMC_ASSERT_EQ_32(%eax, $0x80000000)
    LKMC_ASSERT_EQ_32(%edx, $1)

    # # Division by zero

    # # Division overflow

        # If either

        # - divisor == 0
        # - result > output register size

        # A divide error exception occurs.
        # It then gets handled by the interrupt service 0.

        # Both 0 division and overflow are treated exactly the same!

        # Linux treats this by sending a signal to the process and killing it.

        # Minimal 16-bit example of handling the interrupt:
        # https://github.com/cirosantilli/x86-bare-metal-examples/blob/9e58c1dc656dab54aa69daa38f84eb8c0aa6151e/idt_zero_divide.S

            # Output does not fit into edx.
            #mov eax, 0
            #mov edx, 1
            #mov ecx, 1
            #div ecx

            # Division by zero.
            #mov eax, 1
            #mov edx, 0
            #mov ecx, 0
            #div ecx

        # There is no immediate version:
        # http://stackoverflow.com/questions/4529260/mul-instruction-doesnt-support-an-immediate-value
#endif

LKMC_EPILOGUE
