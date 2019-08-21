#include <lkmc.h>

.global lkmc_start
lkmc_start:
    /* Make all CPUs except CPU0 sleep by default. */
    mrc p15, 0, r0, c0, c0, 5
    ands r0, r0, 3
    bne lkmc_cpu_not_0

    /* Prepare the stack for main, mandatory for C code. */
    ldr sp, =stack_top

    /* Enable floating point.
     * Code copied from: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0409h/CHDEGGFF.html
     * Without this, SIMD operations such as vmov raise an exception.
     */
    mrc p15, 0, r0, c1, c0, 2 /* Read CPACR into r0 */
    orr r0, r0, 3 << 20       /* OR in User and Privileged access for CP10 */
    orr r0, r0, 3 << 22       /* OR in User and Privileged access for CP11 */
    bic r0, r0, 3 << 30       /* Clear ASEDIS/D32DIS if set */
    mcr p15, 0, r0, c1, c0, 2 /* Store new access permissions into CPACR */
    isb                       /* Ensure side-effect of CPACR is visible */
    mov r0, 1 << 30           /* Create value with FPEXC (bit 30) set in r0 */
    vmsr fpexc, r0            /* Enable VFP and SIMD extensions */

    /* https://cirosantilli.com/linux-kernel-module-cheat#magic-failure-string */
    ldr r0, =lkmc_baremetal_on_exit_callback
    bl on_exit

    /* Run main. */
    mov r0, 0
    bl main

    /* If main returns, exit. */
    bl exit

/* Default action for CPUs besides the first one: sleep forever. */
LKMC_WEAK(lkmc_cpu_not_0)
    wfe
    b lkmc_cpu_not_0
