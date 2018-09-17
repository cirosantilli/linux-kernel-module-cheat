.global mystart
mystart:
    ldr sp, =stack_top
    bl main
    b .
