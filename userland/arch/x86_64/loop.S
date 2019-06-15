/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-loop-instruction */

#include <lkmc.h>

LKMC_PROLOGUE

    /* LOOP
     *
     * ....
     * rcx--;
     * if (rcx != 0) goto label
     * ....
     */
    mov $0, %rax
    mov $3, %rcx
loop_label:
    inc %rax
    loop loop_label
    LKMC_ASSERT_EQ(%rax, $3)

    /* LOOPE
     *
     * ....
     * rcx--;
     * if (ecx != 0 && ZF == 1) goto label
     * ....
     *
     * Application: search for first non-zero element in a range.
     *
     * If found, rax will contain the element index.
     *
     * Otherwise, rax contains length + 1.
     */
.section .rodata
    loope_array: .byte 0, 0, 1, 0
.text
    /* Array length. */
    mov $4, %rcx
    mov $-1, %rax
loope_label:
    inc %rax
    cmpb $0, loope_array(%rax)
    loope loope_label
    /* The first non-zero item (1) was at index 2. */
    LKMC_ASSERT_EQ(%rax, $2)

    /* LOOPNE
     *
     * ....
     * ecx--; if (ecx != 0 && ZF == 0) goto lbl
     * ....
     */
LKMC_EPILOGUE
