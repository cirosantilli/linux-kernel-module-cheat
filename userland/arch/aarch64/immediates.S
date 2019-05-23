/* https://github.com/cirosantilli/linux-kernel-module-cheat#gnu-gas-assembler-immediates */

#include <lkmc.h>
LKMC_PROLOGUE
    mov x0, 1
    mov x0, 0x1
    mov x0, 1
    mov x0, 0x1
LKMC_EPILOGUE
