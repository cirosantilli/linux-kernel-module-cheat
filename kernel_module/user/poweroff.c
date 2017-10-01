/* Userspace is for the weak. Die.
 * https://stackoverflow.com/questions/28812514/how-to-shutdown-linux-using-c-or-qt-without-call-to-system
 * BusyBox's /sbin/poweroff is under init/halt.c, but it does extra crap like killing init, so I don't trust it. */

#include <stdio.h>
#include <sys/reboot.h>
#include <unistd.h>

int main(void) {
	puts(__FILE__);
	reboot(RB_POWER_OFF);
}
