/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-rdtsc-instruction */

#include <lkmc.h>

LKMC_PROLOGUE
    rdtsc
    mov %edx, %edi
    shl $32, %rdi
    add %rax, %rdi
    call lkmc_print_hex_64
    call lkmc_print_newline
LKMC_EPILOGUE
