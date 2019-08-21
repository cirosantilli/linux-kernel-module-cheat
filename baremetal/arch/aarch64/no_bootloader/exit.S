/* Test _exit. */

#include <lkmc.h>

.global lkmc_start
lkmc_start:
    mov x0, 0
    bl _exit
