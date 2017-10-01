/* Sleeping beauty, your prince is called Ctrl + C. */

#include <stdio.h>
#include <unistd.h>

int main(void) {
	puts(__FILE__);
	while (1)
		sleep(0xFFFFFFFF);
}
