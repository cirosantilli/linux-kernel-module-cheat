/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-svc-instruction */

#include <lkmc.h>

.global main
main:
    /* Do the svc. */
    svc 0

    /* Confirm that svc was called and modified myvar. */
    ldr x0, myvar
    ldr x1, mynewvar
    cmp x0, x1
    beq 1f
    bl abort
1:

    /* Go home. */
    mov x0, 0
    ret

LKMC_GLOBAL(lkmc_vector_trap_handler)
    /* Modify myvar as a visible side effect. */
    ldr x0, mynewvar
    ldr x1, =myvar
    str x0, [x1]
    ret

myvar:
    .quad 0x0
mynewvar:
    .quad 0x12346789ABCDEF0
