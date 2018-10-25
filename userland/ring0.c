/* https://github.com/cirosantilli/linux-kernel-module-cheat#ring0 */

#include <stdio.h>
#include <stdlib.h>

#include "../include/ring0.h"

int main(void) {
#if defined(__x86_64__) || defined(__i386__)
	Ring0Regs ring0_regs;
	ring0_get_control_regs(&ring0_regs);
#endif
	return EXIT_SUCCESS;
}
