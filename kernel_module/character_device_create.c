/*
Automatically create the device under /dev on insmod, and remove on rmmod.

https://stackoverflow.com/questions/5970595/create-a-device-node-in-code/
https://stackoverflow.com/questions/5970595/how-to-create-a-device-node-from-the-init-module-code-of-a-linux-kernel-module/18594761#18594761
*/

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h> /* register_chrdev, unregister_chrdev */
#include <linux/module.h>
#include <linux/seq_file.h> /* seq_read, seq_lseek, single_release */

#define NAME "lkmc_character_device_create"

static int major = -1;
static struct cdev mycdev;
static struct class *myclass = NULL;

static int show(struct seq_file *m, void *v)
{
	seq_printf(m, "abcd");
	return 0;
}

static int open(struct inode *inode, struct file *file)
{
	return single_open(file, show, NULL);
}

static const struct file_operations fops = {
	.llseek = seq_lseek,
	.open = open,
	.owner = THIS_MODULE,
	.read = seq_read,
	.release = single_release,
};

static int myinit(void)
{
    /* cat /proc/devices */
    alloc_chrdev_region(&major, 0, 1, NAME "_proc");
    /* ls /sys/class */
    myclass = class_create(THIS_MODULE, NAME "_sys");
    /* ls /dev/ */
    device_create(myclass, NULL, major, NULL, NAME "_dev");
    cdev_init(&mycdev, &fops);
    cdev_add(&mycdev, major, 1);
    return 0;
}

static void myexit(void)
{
    device_destroy(myclass, major);
    class_destroy(myclass);
    unregister_chrdev_region(major, 1);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
