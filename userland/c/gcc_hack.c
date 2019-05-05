/* https://github.com/cirosantilli/linux-kernel-module-cheat#your-first-gcc-hack */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int i = 1;
    int j = 1;
    i++;
    j--;
    printf("i = %d\n", i);
    printf("j = %d\n", j);
}
