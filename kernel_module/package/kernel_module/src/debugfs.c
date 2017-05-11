/*
Adapted from: https://github.com/chadversary/debugfs-tutorial/blob/47b3cf7ca47208c61ccb51b27aac6f9f932bfe0b/example1/debugfs_example1.c

Requires CONFIG_DEBUG_FS.

	mkdir /debugfs
	mount -t debugfs none /sys/kernel/debug
	insmod /debugfs.ko
	cd /debugfs/kernel_module_cheat
	cat myfile

Output:

	42
*/

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static struct dentry *dir = 0;
static u32 value = 42;

int init_module(void)
{
    struct dentry *file;
    dir = debugfs_create_dir("kernel_module_cheat", 0);
    if (!dir) {
        printk(KERN_ALERT "debugfs_create_dir failed");
        return -1;
    }
    file = debugfs_create_u32("myfile", 0666, dir, &value);
    if (!file) {
        printk(KERN_ALERT "debugfs_create_u32 failed");
        return -1;
    }
    return 0;
}

void cleanup_module(void)
{
    debugfs_remove_recursive(dir);
}
