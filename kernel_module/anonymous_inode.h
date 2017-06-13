#ifndef IOCTL_H
#define IOCTL_H

#include <linux/ioctl.h>

#define LKMC_ANONYMOUS_INODE_MAGIC 0x33
#define LKMC_ANONYMOUS_INODE_GET_FD _IOR(LKMC_ANONYMOUS_INODE_MAGIC, 0, int)

#endif
