/*
Only tested in x86_64.

Adapted from: https://github.com/dwks/pagemap/blob/8a25747bc79d6080c8b94eac80807a4dceeda57a/pagemap2.c

Dump the page map of a given process PID.

Data sources: /proc/PIC/{map,pagemap}
*/

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#define PAGE_SIZE 0x1000

int main(int argc, char *argv[]) {
	char buffer[BUFSIZ];
	char maps_file[BUFSIZ];
	char pagemap_file[BUFSIZ];
	int maps;
	int offset = 0;
	int pagemap;

	if (argc < 2) {
		printf("Usage: %s pid1\n", argv[0]);
		return EXIT_FAILURE;
	}
	pid_t pid = (pid_t)strtoul(argv[1], NULL, 0);
	snprintf(maps_file, sizeof(maps_file), "/proc/%lu/maps", (unsigned long)pid);
	snprintf(pagemap_file, sizeof(pagemap_file), "/proc/%lu/pagemap", (unsigned long)pid);
	maps = open(maps_file, O_RDONLY);
	if (maps < 0) {
		perror("open maps");
		return EXIT_FAILURE;
	}
	pagemap = open(pagemap_file, O_RDONLY);
	if (pagemap < 0) {
		perror("open pagemap");
		return EXIT_FAILURE;
	}
	printf("addr pfn soft-dirty file/shared swapped present library\n");
	for (;;) {
		ssize_t length = read(maps, buffer + offset, sizeof buffer - offset);
		if (length <= 0) break;
		length += offset;
		for (size_t i = offset; i < (size_t)length; i++) {
			unsigned long low = 0, high = 0;
			if (buffer[i] == '\n' && i) {
				size_t x = i - 1;
				while(x && buffer[x] != '\n') x --;
				if (buffer[x] == '\n') x++;
				size_t beginning = x;
				while(buffer[x] != '-' && x < sizeof buffer) {
					char c = buffer[x++];
					low *= 16;
					if (c >= '0' && c <= '9') {
						low += c - '0';
					}
					else if (c >= 'a' && c <= 'f') {
						low += c - 'a' + 10;
					}
					else break;
				}
				while(buffer[x] != '-' && x < sizeof buffer) x++;
				if (buffer[x] == '-') x++;
				while(buffer[x] != ' ' && x < sizeof buffer) {
					char c = buffer[x++];
					high *= 16;
					if (c >= '0' && c <= '9') {
						high += c - '0';
					}
					else if (c >= 'a' && c <= 'f') {
						high += c - 'a' + 10;
					}
					else break;
				}

				const char *lib_name = 0;
				for (int field = 0; field < 4; field++) {
					x++;
					while(buffer[x] != ' ' && x < sizeof buffer) x++;
				}
				while (buffer[x] == ' ' && x < sizeof buffer) x++;
				size_t y = x;
				while (buffer[y] != '\n' && y < sizeof buffer) y++;
				buffer[y] = 0;
				lib_name = buffer + x;
				{
					unsigned long data;
					for (unsigned long i = low; i < high; i += PAGE_SIZE) {
						unsigned long index = (i / PAGE_SIZE) * sizeof(data);
						if (pread(pagemap, &data, sizeof(data), index) != sizeof(data)) {
							if (errno) perror("pread");
							break;
						}
						printf("%lx %lx %d %d %d %d %s\n",
							i,
							data & 0x7fffffffffffff,
							(data >> 55) & 1,
							(data >> 61) & 1,
							(data >> 62) & 1,
							(data >> 63) & 1,
							lib_name
						);
					}
				}
			}
		}
	}
	close(maps);
	close(pagemap);
	return 0;
}
