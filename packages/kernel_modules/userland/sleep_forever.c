/* https://github.com/cirosantilli/linux-kernel-module-cheat#sleep_forever-out */

#include <stdio.h>
#include <unistd.h>

int main(void) {
	puts(__FILE__);
	while (1)
		sleep(0xFFFFFFFF);
}
