/* https://cirosantilli.com/linux-kernel-module-cheat#arm-wfe-and-sev-instructions */

.text
.global _start
_start:
asm_main_after_prologue:
    sevl
    /* Shoul not sleep due to above sevl. */
    wfe

    /* exit */
    mov x0, 0     /* exit status */
    mov x8, 93    /* syscall number */
    svc 0
