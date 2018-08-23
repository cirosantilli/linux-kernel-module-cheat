/* https://github.com/cirosantilli/linux-kernel-module-cheat#poweroff-out */

#define _XOPEN_SOURCE 700
#include <sys/reboot.h>
#include <unistd.h>

int main(void) {
	reboot(RB_POWER_OFF);
}
