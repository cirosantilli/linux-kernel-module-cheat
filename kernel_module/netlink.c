/*
https://en.wikipedia.org/wiki/Netlink

https://stackoverflow.com/questions/3299386/how-to-use-netlink-socket-to-communicate-with-a-kernel-module
*/

#include <linux/delay.h> /* usleep_range */
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>

/* Socket identifier, matches userland. TODO can be anything?
 * Is there a more scalable way to do it? E.g. ioctl device,
 * kernel generates one on the fly, then give it back and connect?
 * https://stackoverflow.com/questions/32898173/can-i-have-more-than-32-netlink-sockets-in-kernelspace */
#define NETLINK_USER 31

struct sock *nl_sk = NULL;

static void callback(struct sk_buff *skb)
{
	char readbuf[1024];
	size_t readbuflen;
    int pid;
    int res;
    struct nlmsghdr *nlh;
    struct sk_buff *skb_out;

	/* Read user message. */
    nlh = (struct nlmsghdr *)skb->data;
    pr_info("kernel received: %s\n", (char *)nlmsg_data(nlh));

	/* Add an artificial sleep to see what happens when
	 * multiple requests come in at the same time.
	 *
	 * Try this out (it works):
	 * for i in `seq 16`; do /netlink.out & done */
	usleep_range(1000000, 1000001);

	/* Reply with jiffies. */
	readbuflen = snprintf(readbuf, sizeof(readbuf), "%llu", (unsigned long long)jiffies);
    pid = nlh->nlmsg_pid;
    skb_out = nlmsg_new(readbuflen, 0);
    if (!skb_out) {
        pr_err("nlmsg_new\n");
        return;
    }
    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, readbuflen, 0);
    NETLINK_CB(skb_out).dst_group = 0;
    strncpy(nlmsg_data(nlh), readbuf, readbuflen);
    res = nlmsg_unicast(nl_sk, skb_out, pid);
    if (res < 0)
        pr_info("nlmsg_unicast\n");
}

static int myinit(void)
{
    struct netlink_kernel_cfg cfg = {
        .input = callback,
    };
    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk) {
        pr_err("netlink_kernel_create\n");
        return -10;
    }
    return 0;
}

static void myexit(void)
{
    netlink_kernel_release(nl_sk);
}

module_init(myinit);
module_exit(myexit);
MODULE_LICENSE("GPL");
