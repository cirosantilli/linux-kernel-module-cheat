/* https://cirosantilli.com/linux-kernel-module-cheat#c-empty-struct */

#include <assert.h>
#include <stdlib.h>

int main(void) {
    struct s {};
    struct s s0;
    assert(sizeof(s0) == 0);
    return EXIT_SUCCESS;
}
