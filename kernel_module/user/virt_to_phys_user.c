/*
Convert a virtual address to physical for a given process PID using /proc/PID/pagemap.

https://stackoverflow.com/questions/5748492/is-there-any-api-for-determining-the-physical-address-from-virtual-address-in-li/45128487#45128487
*/

#include <stdio.h> /* printf */
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE, strtoull */

#include "common.h" /* virt_to_phys_user */

int main(int argc, char **argv)
{
	pid_t pid;
	uintptr_t vaddr, paddr;

	if (argc < 3) {
		printf("Usage: %s pid vaddr\n", argv[0]);
		return EXIT_FAILURE;
	}
	pid = strtoull(argv[1], NULL, 0);
	vaddr = strtoull(argv[2], NULL, 0);
	if (virt_to_phys_user(&paddr, pid, vaddr)) {
		fprintf(stderr, "error: virt_to_phys_user\n");
		return EXIT_FAILURE;
	};
	printf("0x%jx\n", (uintmax_t)paddr);
	return EXIT_SUCCESS;
}
