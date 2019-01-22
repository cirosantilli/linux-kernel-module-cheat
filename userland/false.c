/* Test that emulators forward the exit status properly. */

#include <stdlib.h>

int main(int argc, char **argv) {
    int ret;
    if (argc == 1) {
        ret = 1;
    } else {
        ret = strtoull(argv[1], NULL, 0);
    }
	return ret;
}
