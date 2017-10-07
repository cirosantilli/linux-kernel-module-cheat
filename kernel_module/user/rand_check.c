/* Check if we were able to remove certain sources of randomness
 * across boots using different techniques:
 *
 * - QEMU icount record replay
 * - norandmaps boot parameter
 *
 * You might want to run this as the init process to further remove undeterminism. */

#include <inttypes.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int bss = 0;
int data = 1;

int main(__attribute__((unused)) int argc, char **argv) {
	int i, *ip;
	uint64_t uint64;
	FILE *fp;

	printf("time(NULL) = %ju\n", (uintmax_t)time(NULL));
	printf("&i         = %p\n", (void *)&i);
	printf("&argv[0]   = %p\n", (void *)&argv[0]);
	printf("&main      = %p\n", (void *)(intptr_t)main);
	printf("&bss       = %p\n", (void *)&bss);
	printf("&data      = %p\n", (void *)&data);

	/* malloc */
	ip = malloc(sizeof(*ip));
	printf("&malloc    = %p\n", (void *)ip);
	free(ip);

	/* /dev/urandom */
	fp = fopen("/dev/urandom", "rb");
	fread(&uint64, sizeof(uint64), 1, fp);
	printf("/dev/urandom = %" PRIx64 "\n", uint64);
	fclose(fp);
}
