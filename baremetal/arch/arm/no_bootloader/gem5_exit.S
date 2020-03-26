.global _start
_start:
    mov r0, #0; mov r1, #0; .inst 0xEE000110 | (0x21 << 16);
