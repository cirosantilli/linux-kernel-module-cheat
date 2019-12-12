/* https://cirosantilli.com/linux-kernel-module-cheat#arm-wfe-and-sev-instructions */
.global _start
_start:
    wfe
    mov x0, 0
    bl exit
