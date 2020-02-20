/* https://cirosantilli.com/linux-kernel-module-cheat#benchmark-emulators-on-userland-executables */

#define LKMC_M5OPS_ENABLE 1
#include "lkmc/m5ops.h"

.text
.global _start
_start:
asm_main_after_prologue:
    LKMC_M5OPS_EXIT_ASM
