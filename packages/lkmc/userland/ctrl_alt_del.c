/* https://github.com/cirosantilli/linux-kernel-module-cheat#ctrl-alt-del */

#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/reboot.h>
#include <unistd.h>

void signal_handler(int sig) {
	write(STDOUT_FILENO, "cad\n", 4);
	signal(sig, signal_handler);
}

int main(void) {
	int i = 0;
	/* Disable the forced reboot, enable sending SIGINT to init. */
	reboot(RB_DISABLE_CAD);
    signal(SIGINT, signal_handler);
	while (1) {
		sleep(1);
		printf("%d\n", i);
		i++;
	}
    return EXIT_SUCCESS;
}
