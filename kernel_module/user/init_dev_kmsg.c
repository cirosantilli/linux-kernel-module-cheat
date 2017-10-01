/* https://stackoverflow.com/questions/12683169/measure-time-taken-for-linux-kernel-from-bootup-to-userpace/46517014#46517014 */

#include <stdio.h>
#include <unistd.h>

int main(void) {
    FILE *fp;
    fp = fopen("/dev/kmsg", "w");
	fputs(__FILE__ "\n", fp);
	fclose(fp);
	while (1)
		sleep(0xFFFFFFFF);
}
