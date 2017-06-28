/*
https://stackoverflow.com/questions/12040303/accessing-physical-address-from-user-space

Sample call:

	/phys.out 0 16

Confirm memory from QEMU monitor with:

	xp/0 16
*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage: %s <phys_addr> <len>\n", argv[0]);
		return 0;
	}
	off_t offset = strtoul(argv[1], NULL, 0);
	size_t len = strtoul(argv[2], NULL, 0);

	size_t pagesize = sysconf(_SC_PAGE_SIZE);
	off_t page_base = (offset / pagesize) * pagesize;
	off_t page_offset = offset - page_base;
	int fd = open("/dev/mem", O_SYNC);
	size_t reallen = page_offset + len;
	unsigned char *mem = mmap(NULL, reallen, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, page_base);
	if (mem == MAP_FAILED) {
		/* TODO why does it fail for some addreses but not for others? */
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	for (size_t i = 0; i < len; ++i) {
		printf("%02x ", (unsigned int)mem[page_offset + i]);
		/* TODO can't edit memory? */
		mem[page_offset + i] = i % 256;
	}
	puts("");
	if (munmap(mem, reallen) == -1) {
		perror("munmap");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}
