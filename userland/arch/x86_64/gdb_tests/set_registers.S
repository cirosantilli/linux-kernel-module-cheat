/* Test that we can set registers from GDB. */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $1, %rax
    /* test-gdb-rax */
    mov $2, %rbx
    /* test-gdb-rbx */
LKMC_EPILOGUE
