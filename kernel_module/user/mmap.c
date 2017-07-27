#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE 4096

int main(int argc, char **argv)
{
	int fd;
	char *address = NULL;

	if (argc < 2) {
		printf("Usage: %s <mmap_file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	printf("open pathname = %s\n", argv[1]);
	fd = open(argv[1], O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("open");
		return EXIT_FAILURE;
	}
	printf("fd = %d\n", fd);
	address = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (address == MAP_FAILED) {
		perror("mmap");
		return EXIT_FAILURE;
	}
	printf("%s\n", address);
	memcpy(address + 11 , "qwer", 6);
	printf("%s\n", address);
	close(fd);
	return EXIT_SUCCESS;
}
