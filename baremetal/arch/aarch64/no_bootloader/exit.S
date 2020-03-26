/* Test _exit. */

#include <lkmc.h>

.global _start
_start:
    mov x0, 0
    bl _exit
