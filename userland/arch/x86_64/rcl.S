/* https://cirosantilli.com/linux-kernel-module-cheat#x86-shift-and-rotate-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $0x81, %r12
    clc

    rcl $1, %r12b
    /* We'll have to save and restore flags across our asserts!
        * 2x PUSHF to maintain 16-bit stack alignment.
        * https://cirosantilli.com/linux-kernel-module-cheat#x86_64-calling-convention
        */
    pushf
    pushf
    LKMC_ASSERT(jc)
    LKMC_ASSERT_EQ(%r12, $2)

    popf
    rcl $1, %r12b
    pushf
    LKMC_ASSERT(jnc)
    LKMC_ASSERT_EQ(%r12, $5)

    popf
    rcr $2, %r12b
    pushf
    LKMC_ASSERT(jnc)
    LKMC_ASSERT_EQ(%r12, $0x81)

    popf
    rcr $1, %r12b
    pushf
    LKMC_ASSERT(jc)
    LKMC_ASSERT_EQ(%r12, $0x40)

    add $16, %rsp
LKMC_EPILOGUE
