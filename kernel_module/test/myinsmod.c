#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define init_module(mod, len, opts) syscall(__NR_init_module, mod, len, opts)

int main(int argc, char **argv) {
	size_t image_size;
	void *image;
	int fd;
	struct stat st;

	if (argc != 2) {
		puts("Usage ./prog mymodule.ko");
		return EXIT_FAILURE;
	}
	fd = open(argv[1], O_RDONLY);
	fstat(fd, &st);
	image_size = st.st_size;
	image = malloc(image_size);
	read(fd, image, image_size);
	close(fd);
	if (init_module(image, image_size, "") != 0) {
		perror("init_module");
		return EXIT_FAILURE;
	}
	free(image);
	return EXIT_SUCCESS;
}
