/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-multicore */

.global main
main:
    /* Reset spinlock. */
    mov x0, #0
    ldr x1, =spinlock
    str x0, [x1]

    /* Read cpu id into x1. */
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, 1f
    /* Only CPU 1 reaches this point and sets the spinlock. */
    mov x0, #1
    ldr x1, =spinlock
    str x0, [x1]
    b .
1:
    /* Only CPU 0 reaches this point. */
    ldr x0, spinlock
    cbz x0, 1b

    ret

spinlock:
    .skip 8
