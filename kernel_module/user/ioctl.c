#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../ioctl.h"

int main(int argc, char **argv)
{
	int fd, arg_int, ret;
	lkmc_ioctl_struct arg_struct;

	if (argc < 4) {
		puts("Usage: ./prog <ioctl-file> <cmd> <arg>");
		return EXIT_FAILURE;
	}
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("open");
		return EXIT_FAILURE;
	}
	/* 0 */
	{
		arg_int = 1;
		ret = ioctl(fd, LKMC_IOCTL_INC, &arg_int);
		if (ret == -1) {
			perror("ioctl");
			return EXIT_FAILURE;
		}
		printf("arg = %d\n", arg_int);
		printf("ret = %d\n", ret);
		printf("errno = %d\n", errno);
	}
	puts("");
	/* 1 */
	{
		arg_struct.i = 1;
		arg_struct.j = 1;
		ret = ioctl(fd, LKMC_IOCTL_INC_DEC, &arg_struct);
		if (ret == -1) {
			perror("ioctl");
			return EXIT_FAILURE;
		}
		printf("arg = %d %d\n", arg_struct.i, arg_struct.j);
		printf("ret = %d\n", ret);
		printf("errno = %d\n", errno);
	}
	close(fd);
	return EXIT_SUCCESS;
}
