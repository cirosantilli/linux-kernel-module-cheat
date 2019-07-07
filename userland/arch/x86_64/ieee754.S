/* https://cirosantilli.com/linux-kernel-module-cheat#ieee-754 */

#include <lkmc.h>
#include <lkmc/float.h>

.data
    double_1_5: .quad LKMC_FLOAT_64(0x0, 0x0, 0x8000000000000)
    double_2_5: .quad LKMC_FLOAT_64(0x0, 0x1, 0x4000000000000)
    double_4_0: .quad LKMC_FLOAT_64(0x0, 0x2, 0x0000000000000)
    double_minus_4_0: .quad LKMC_FLOAT_64(0x1, 0x2, 0x10000000000000)
    double_plus_infinity: .quad LKMC_FLOAT_64_PLUS_INFINITY
    double_nan: .quad LKMC_FLOAT_64_QNAN_DEFAULT
    double_ref_1_5: .double 1.5
    double_ref_2_5: .double 2.5
    double_ref_4_0: .double 4.0
    double_ref_minus_4_0: .double -4.0
    double_ref_plus_infinity: .double inf
    double_ref_nan: .double nan
LKMC_PROLOGUE
    /* Check that our macros are correct. */
    LKMC_ASSERT_EQ(double_1_5, double_ref_1_5)
    LKMC_ASSERT_EQ(double_2_5, double_ref_2_5)
    LKMC_ASSERT_EQ(double_4_0, double_ref_4_0)
    LKMC_ASSERT_EQ(double_minus_4_0, double_ref_minus_4_0)
    LKMC_ASSERT_EQ(double_plus_infinity, double_ref_plus_infinity)
    /* TODO: GAS nan is a different NaN: 0x7FFFFFFF */
    /*LKMC_ASSERT_EQ(double_nan, double_ref_nan)*/

    /* x87 80-bit FPU: https://cirosantilli.com/linux-kernel-module-cheat#x86-x87-fpu-instructions */

        /* 1.5 + 2.5 == 4.0. */
        fldl double_1_5
        fldl double_2_5
        faddp %st, %st(1)
        fldl double_4_0
        fcomip %st(1)
        LKMC_ASSERT(je)

        /* 4.0 + -4.0 == 0.0. */
        fldl double_minus_4_0
        faddp %st, %st(1)
        fldz
        fcomip %st(1)
        LKMC_ASSERT(je)

        /* 0.0 + inf == inf */
        fldl double_plus_infinity
        faddp %st, %st(1)
        fldl double_plus_infinity
        fcomip %st(1)
        LKMC_ASSERT(je)

        /*  inf + nan == nan */
        fldl double_nan
        faddp %st, %st(1)
        fldl double_nan
        fcomip %st(1)
        LKMC_ASSERT(je)
LKMC_EPILOGUE
