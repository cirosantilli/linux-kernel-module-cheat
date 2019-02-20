#include <lkmc.h>

.global mystart
mystart:
    /* = NEON setup */
    mov x1, #(0x3 << 20)
    msr cpacr_el1, x1
    isb

    /* Load the vector table. */
    ldr x0, =lkmc_vector_table
    msr vbar_el1, x0

    /* Prepare the stack for main, mandatory for C code. */
    ldr x0, =stack_top
    mov sp, x0
    bl main

    /* If main returns, exit. */
    bl exit

LKMC_VECTOR_TABLE

/* Default trap handler. */
LKMC_WEAK(lkmc_vector_trap_handler)
    ldr x0, =lkmc_vector_trap_handler_error
    bl puts
    bl lkmc_assert_fail
lkmc_vector_trap_handler_error:
    .asciz "error: unexpected interrupt"
