#define _XOPEN_SOURCE 700
#include <fcntl.h> /* creat, O_CREAT */
#include <poll.h> /* poll */
#include <stdio.h> /* printf, puts, snprintf */
#include <stdlib.h> /* EXIT_FAILURE, EXIT_SUCCESS */
#include <unistd.h> /* read */

int main(int argc, char **argv) {
    char buf[1024];
    int fd, n;
    short revents;
    struct pollfd pfd;

    fd = open(argv[1], O_RDONLY | O_NONBLOCK);
    pfd.fd = fd;
    pfd.events = POLLIN;
    while (1) {
        puts("loop");
        poll(&pfd, 1, -1);
        revents = pfd.revents;
        if (revents & POLLIN) {
            n = read(pfd.fd, buf, sizeof(buf));
            printf("POLLIN n=%d buf=%.*s\n", n, n, buf);
        }
        if (revents & POLLHUP) {
            printf("POLLHUP\n");
            close(pfd.fd);
            break;
        }
        if (revents & POLLNVAL) {
            printf("POLLNVAL\n");
        }
        if (revents & POLLERR) {
            printf("POLLERR\n");
        }
    }
}
