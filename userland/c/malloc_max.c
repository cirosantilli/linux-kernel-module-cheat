/* https://cirosantilli.com/linux-kernel-module-cheat#malloc-maximum-size */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char *ptr = NULL;
    size_t size = 1;
    while (1) {
        printf("0x%zx\n", size);
        ptr = realloc(ptr, size);
        if (ptr == NULL) {
            break;
        } else {
            size <<= 1;
        }
    }
}
