/* MInimal sanity check of the C driver. */
.text
.global asm_main
asm_main:
asm_main_after_prologue:
    mov w0, 0
    ret
