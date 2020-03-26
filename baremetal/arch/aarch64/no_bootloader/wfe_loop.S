/* https://cirosantilli.com/linux-kernel-module-cheat#gem5-simulate-limit-reached */

#include <lkmc/m5ops.h>

.global _start
_start:
    wfe
    b _start
