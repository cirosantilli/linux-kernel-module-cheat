/* https://cirosantilli.com/linux-kernel-module-cheat#gem5-checkpoint-userland-minimal-example */

#define LKMC_M5OPS_ENABLE 1
#include "lkmc/m5ops.h"

.text
.global _start
_start:
asm_main_after_prologue:
    LKMC_M5OPS_CHECKPOINT_ASM
    LKMC_M5OPS_EXIT_ASM
