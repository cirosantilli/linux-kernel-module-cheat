#include <lkmc.h>

.global lkmc_start
lkmc_start:
    /* Load the vector table. */
    ldr x0, =lkmc_vector_table
    msr vbar_el1, x0

    /* https://cirosantilli.com/linux-kernel-module-cheat#aarch64-baremetal-neon-setup */
    mov x1, 0x3 << 20
    msr cpacr_el1, x1
    isb

    /* Prepare the stack for main, mandatory for C code. */
    ldr x0, =stack_top
    mov sp, x0

    /* https://cirosantilli.com/linux-kernel-module-cheat#magic-failure-string */
    adr x0, lkmc_baremetal_on_exit_callback
    bl on_exit

    /* Run main. */
    mov x0, 0
    bl main

    /* If main returns, exit. */
    bl exit

LKMC_VECTOR_TABLE

/* Default trap handler. */
LKMC_WEAK(lkmc_vector_trap_handler)
    ldr x0, =lkmc_vector_trap_handler_error_message
    bl puts
    bl abort
lkmc_vector_trap_handler_error_message:
    .asciz "error: unexpected interrupt"
