/* https://cirosantilli.com/linux-kernel-module-cheat#x86-cpuid-instruction */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $0, %eax
    cpuid

    /* Save the other registers. */
    mov %ebx, %r12d
    mov %ecx, %r13d
    mov %edx, %r14d

    /* eax */
    mov %eax, %edi
    call lkmc_print_hex_32
    call lkmc_print_newline

    /* ebx */
    mov %r12d, %edi
    call lkmc_print_hex_32
    call lkmc_print_newline

    /* ecx */
    mov %r13d, %edi
    call lkmc_print_hex_32
    call lkmc_print_newline

    /* edx */
    mov %r14d, %edi
    call lkmc_print_hex_32
    call lkmc_print_newline
LKMC_EPILOGUE
