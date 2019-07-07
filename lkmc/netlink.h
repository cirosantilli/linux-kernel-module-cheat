/* https://cirosantilli.com/linux-kernel-module-cheat#netlink-sockets */

#ifndef LKMC_NETLINK_H
#define LKMC_NETLINK_H

/* Socket identifier, matches userland. TODO can be anything?
 * Is there a more scalable way to do it? E.g. ioctl device,
 * kernel generates one on the fly, then give it back and connect?
 * https://stackoverflow.com/questions/32898173/can-i-have-more-than-32-netlink-sockets-in-kernelspace */
#define NETLINK_USER 31

#endif
