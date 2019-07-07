/* https://cirosantilli.com/linux-kernel-module-cheat#netlink-sockets */

#include <linux/netlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <lkmc/netlink.h>

#define MAX_PAYLOAD 1024

/* Some of these structs fields must be zeroed.
 * We could brute force memset them, but
 * TODO determine exactly which, and move into main. */
int sock_fd;
struct iovec iov;
struct msghdr msg;
struct nlmsghdr *nlh;
struct sockaddr_nl src_addr, dest_addr;

int main(void) {
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
    dest_addr.nl_family = AF_NETLINK;
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;
    strcpy(NLMSG_DATA(nlh), "user request");
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    fprintf(stderr, "before sendmsg\n");
    sendmsg(sock_fd, &msg, 0);
    fprintf(stderr, "after sendmsg\n");
    recvmsg(sock_fd, &msg, 0);
    printf("%s\n", (char *)NLMSG_DATA(nlh));
    close(sock_fd);
}
