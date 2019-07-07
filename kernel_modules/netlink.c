/* https://cirosantilli.com/linux-kernel-module-cheat#netlink-sockets */

#include <linux/delay.h> /* usleep_range */
#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>

#include <lkmc/netlink.h>

struct sock *nl_sk = NULL;

static u32 count;
static u32 sleep;
module_param(sleep, int, S_IRUSR | S_IWUSR);

static void callback(struct sk_buff *skb)
{
	char readbuf[9];
	size_t readbuflen;
	int pid;
	int res;
	struct nlmsghdr *nlh;
	struct sk_buff *skb_out;

	nlh = (struct nlmsghdr *)skb->data;
	pr_info("kernel received: %s\n", (char *)nlmsg_data(nlh));
	if (sleep)
		usleep_range(1000000, 1000001);
	readbuflen = snprintf(readbuf, sizeof(readbuf), "%x", count);
	count++;
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
