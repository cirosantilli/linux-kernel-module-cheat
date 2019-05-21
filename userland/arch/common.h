/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly-c-standard-library */

#ifndef COMMON_H
#define COMMON_H

/* We define in this header only macros that are the same on all archs. */

/* common_arch.h contains arch specific macros. */
#include "common_arch.h"

.extern \
    exit, \
    printf, \
    puts \
;

/* Assert that the given branch instruction is taken. */
#define LKMC_ASSERT(branch_if_pass) \
    branch_if_pass 1f; \
    LKMC_FAIL; \
1: \
;

#ifndef LKMC_ASSERT_EQ_REG
/* Assert that a register equals another register. */
#define LKMC_ASSERT_EQ_REG(reg1, reg2) \
    cmp reg1, reg2; \
    LKMC_ASSERT(beq); \
;
#endif

#endif
