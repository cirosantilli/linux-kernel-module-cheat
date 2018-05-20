/* Userspace is for the weak. Die.
 * https://stackoverflow.com/questions/28812514/how-to-shutdown-linux-using-c-or-qt-without-call-to-system
 **/

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <sys/reboot.h>
#include <unistd.h>

int main(void) {
	puts(__FILE__);
	reboot(RB_POWER_OFF);
}
