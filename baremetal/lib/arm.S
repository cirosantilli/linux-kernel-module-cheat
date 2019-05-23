#include <lkmc.h>

.global mystart
mystart:
    /* Prepare the stack for main, mandatory for C code. */
    ldr sp, =stack_top

    /* https://github.com/cirosantilli/linux-kernel-module-cheat#magic-failure-string */
    ldr r0, =lkmc_baremetal_on_exit_callback
    bl on_exit

    /* Run main. */
    mov r0, 0
    bl main

    /* If main returns, exit. */
    bl exit
