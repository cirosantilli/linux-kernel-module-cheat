/* https://cirosantilli.com/linux-kernel-module-cheat#x86-rdtscp-instruction */

#include <lkmc.h>

LKMC_PROLOGUE
    rdtscp
    mov %edx, %edi
    shl $32, %rdi
    add %rax, %rdi
    mov %ecx, %r12d

    /* Print RDTSC. */
    call lkmc_print_hex_64
    call lkmc_print_newline

    /* Print PID. */
    mov %r12d, %edi
    call lkmc_print_hex_32
    call lkmc_print_newline
LKMC_EPILOGUE
