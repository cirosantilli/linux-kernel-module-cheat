/* https://cirosantilli.com/linux-kernel-module-cheat#arm-wfe-and-sev-instructions */

.text
.global _start
_start:
asm_main_after_prologue:
    wfe
    wfe

    /* exit */
    mov x0, 0
    mov x8, 93
    svc 0
