/* https://cirosantilli.com/linux-kernel-module-cheat#x86-logical-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $0x00FF, %rax
    xor $0x0F0F, %ax
    LKMC_ASSERT_EQ(%rax, $0x0FF0)

    /* xor to set to zero idiom
     *
     * http://stackoverflow.com/questions/1135679/does-using-xor-reg-reg-give-advantage-over-mov-reg-0
     *
     * xor can be used to set a register to 0 instad of mov:
     *
     * Compileres often do this to generate shorter instrucitons,
     * but there are also cases where `mov` is better.
     */
    mov $0x1020, %rax
    xor %ax, %ax
    mov $0x0000, %ax
LKMC_EPILOGUE
