#define _XOPEN_SOURCE 700
#include <fcntl.h> /* creat, O_CREAT */
#include <poll.h> /* poll */
#include <stdio.h> /* printf, puts, snprintf */
#include <stdlib.h> /* EXIT_FAILURE, EXIT_SUCCESS */
#include <unistd.h> /* read */

int main(int argc, char **argv) {
	char buf[1024], path[1024];
	int fd, i, n;
	short revents;
	struct pollfd pfd;

	fd = open(argv[1], O_RDONLY | O_NONBLOCK);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	pfd.fd = fd;
	pfd.events = POLLIN;
	while (1) {
		puts("loop");
		i = poll(&pfd, 1, -1);
		if (i == -1) {
			perror("poll");
			exit(EXIT_FAILURE);
		}
		revents = pfd.revents;
		if (revents & POLLIN) {
			n = read(pfd.fd, buf, sizeof(buf));
			printf("POLLIN n=%d buf=%.*s\n", n, n, buf);
		}
	}
}
