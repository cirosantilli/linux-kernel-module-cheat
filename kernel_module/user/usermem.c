/*
Only tested in x86_64.

Provide an allocated userland memory for us to test out kernel memory APIs, including:

- /proc/pid/maps
- /proc/pid/pagemap. See also: https://stackoverflow.com/questions/17021214/decode-proc-pid-pagemap-entry/45126141#45126141
- /dev/mem

Usage:

	/usermem.out &

Outputs the virtual address and pid, e.g.:

	vaddr 0x600800
	pid 110

Translate the virtual address to physical for the given PID:

	/virt_to_phys_user.out 110 0x600800

Sample output physical address:

	0x7c7b800

## QEMU monitor xp

Examine the physical memory from the QEMU monitor: on host:

	./qemumonitor
	xp 0x7c7b800

Output:

	0000000007c7b800: 0x12345678

Yes!!! We read the correct value from the physical address.

## /dev/mem

Firts up, this requires:

- CONFIG_STRICT_DEVMEM is not set.
- nopat on kernel parameters

see: https://stackoverflow.com/questions/11891979/how-to-access-mmaped-dev-mem-without-crashing-the-linux-kernel

Then:

	devmem2 0x7c7b800

Possible output:

	Memory mapped at address 0x7ff7dbe01000.
	Value at address 0X7C7B800 (0x7ff7dbe01800): 0x12345678

where 0x7ff7dbe01000 is a new virtual address that was mapped
to our physical address and given to the process that mapped /dev/mem.

And finally, let's change the value!

	devmem2 0x7c7b800 w 0x9abcdef0

After one second, we see on the screen:

	i 9abcdef0
	[1]+  Done                       /usermem.out

so the while loop was exited!
*/

#define _XOPEN_SOURCE 700
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

enum { I0 = 0x12345678 };

static volatile uint32_t i = I0;

int main(void) {
	printf("vaddr %p\n", (void *)&i);
	printf("pid %ju\n", (uintmax_t)getpid());
	while (i == I0) {
		sleep(1);
	}
	printf("i %jx\n", (uintmax_t)i);
	return EXIT_SUCCESS;
}
