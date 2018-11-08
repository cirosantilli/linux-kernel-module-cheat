.global mystart
mystart:
    ldr sp, =stack_top
    bl main
    mov r0, #0
    bl exit
