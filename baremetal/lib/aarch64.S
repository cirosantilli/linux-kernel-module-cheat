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

/* Default trap handler that does nothing.
 *
 * Weak means that if any other file defines it as a non-weak global,
 * that one will take precedence.
 *
 * We need this one to not get undefined references.
 */
LKMC_WEAK(lkmc_vector_trap_handler)
    ret
