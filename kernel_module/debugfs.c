/*
Adapted from: https://github.com/chadversary/debugfs-tutorial/blob/47b3cf7ca47208c61ccb51b27aac6f9f932bfe0b/example1/debugfs_example1.c

Usage:

	/debugfs.sh

Requires `CONFIG_DEBUG_FS=y`.
*/

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static struct dentry *dir;
static u32 value = 42;

static int myinit(void)
{
    struct dentry *file;
    dir = debugfs_create_dir("lkmc_debugfs", 0);
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

static void myexit(void)
{
    debugfs_remove_recursive(dir);
}

module_init(myinit)
module_exit(myexit)
