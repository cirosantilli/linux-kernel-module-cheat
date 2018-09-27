.global mystart
mystart:
    mov x0, #0; .inst 0XFF000110 | (0x21 << 16);
