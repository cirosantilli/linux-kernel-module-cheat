#include <lkmc.h>

.global _start
_start:
    /* Make all CPUs except CPU0 sleep by default. */
    mrs x0, mpidr_el1
    ands x0, x0, 3
    bne lkmc_cpu_not_0

    /* Load the vector table. */
    ldr x0, =lkmc_vector_table
    msr vbar_el1, x0

    /* https://cirosantilli.com/linux-kernel-module-cheat#aarch64-baremetal-neon-setup */
    /* CPACR_EL1.FPEN */
    mov x1, 0x3 << 20
    /* CPACR_EL1.ZEN */
    orr x1, x1, 0x3 << 16
    msr cpacr_el1, x1
    isb

    /* Prepare the stack for main, mandatory for C code. */
    ldr x0, =lkmc_stack_top
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

/* Default handler for exceptions. This is called after some basic
 * setup done on the initial handler. Since this is a weak symbol,
 * you can redefine it in your own example, and your definition
 * will take precedence. */
LKMC_WEAK(lkmc_vector_trap_handler)
    ldr x0, =lkmc_vector_trap_handler_error_message
    bl puts
    bl abort
lkmc_vector_trap_handler_error_message:
    .asciz "error: unexpected interrupt"

/* Default action for CPUs besides the first one: sleep forever. */
LKMC_WEAK(lkmc_cpu_not_0)
    wfe
    b lkmc_cpu_not_0
