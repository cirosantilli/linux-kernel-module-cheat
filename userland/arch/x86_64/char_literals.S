/* https://cirosantilli.com/linux-kernel-module-cheat#gnu-gas-assembler-char-literals */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $0, %r12

    /* Memory. */
    mov mychar, %r12b
    LKMC_ASSERT_EQ(%r12, $0x61)

    /* Immediate. Without the `$`, does a memory access, and segfaults! */
    mov $'b, %r12b
    LKMC_ASSERT_EQ(%r12, $0x62)

    /* Space character works. */
    mov $' , %r12b
    LKMC_ASSERT_EQ(%r12, $0x20)

    /* Backslash escapes work. */
    mov $'\n , %r12b
    LKMC_ASSERT_EQ(%r12, $0x0A)
LKMC_EPILOGUE
mychar:
    .byte 'a
