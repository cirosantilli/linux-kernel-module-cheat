.global main
main:
    /* SYS_EXIT */
    mov r0, #0x18
    /* ADP_Stopped_ApplicationExit */
    ldr r1, =#0x20026
    /* Do the semihosting call on A32. */
    svc 0x00123456
