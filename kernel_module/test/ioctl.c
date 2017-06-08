#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv)
{
	int fd, ret;

	if (argc < 4) {
		puts("Usage: ./prog <ioctl-file> <cmd> <arg>");
		return EXIT_FAILURE;
	}
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("open");
		return EXIT_FAILURE;
	}
	ret = ioctl(fd, strtoul(argv[2], NULL, 10), argv[3]);
	if (ret == -1) {
		perror("ioctl");
		return EXIT_FAILURE;
	}
	printf("ret = %d\n", ret);
	printf("errno = %d\n", errno);
	close(fd);
	return EXIT_SUCCESS;
}
