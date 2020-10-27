/* https://cirosantilli.com/linux-kernel-module-cheat#m5ops-magic-addresses */

.global _start
_start:
    # First parameter.
    mov x0, 0
    # 0x21 is exit.
    # 0x10010000 is system.m5ops_base
    ldr x10, =(0x21 << 8)
    ldr x9, =0x10010000
    # Do the magic access.
    ldr x0, [x9, x10]
