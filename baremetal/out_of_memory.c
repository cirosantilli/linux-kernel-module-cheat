/* Test out of memory. */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char *ptr = NULL;
    size_t alloc_size = 1;
    while (1) {
        ptr = realloc(ptr, alloc_size);
        if (ptr == NULL) {
            puts("out of memory");
            break;
        } else {
            alloc_size <<= 1;
        }
    }
}
