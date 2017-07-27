/*
Adapted from: https://github.com/t3rm1n4l/kern-dev-tutorial/blob/1f036ef40fc4378f5c8d2842e55bcea7c6f8894a/05-sysfs/sysfs.c

Vs procfs:

- https://unix.stackexchange.com/questions/4884/what-is-the-difference-between-procfs-and-sysfs
- https://stackoverflow.com/questions/37237835/how-to-attach-file-operations-to-sysfs-attribute-in-platform-driver

This example shows how sysfs is more restricted, as it does not take a file_operations.

So you basically can only do open, close, read, write, and lseek on sysfs files.

It is kind of similar to a seq_file file_operations, except that write is also implemented.

TODO: what are those kobject structs? Make a more complex example that shows what they can do.

- https://www.kernel.org/doc/Documentation/kobject.txt
- https://www.quora.com/What-are-kernel-objects-Kobj
- http://www.makelinux.net/ldd3/chp-14-sect-1
- https://www.win.tue.nl/~aeb/linux/lk/lk-13.html
*/

#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <uapi/linux/stat.h> /* S_IRUSR, S_IWUSR  */

enum { FOO_SIZE_MAX = 4 };
static int foo_size;
static char foo_tmp[FOO_SIZE_MAX];

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buff)
{
	strncpy(buff, foo_tmp, foo_size);
	return foo_size;
}

static ssize_t foo_store(struct  kobject *kobj, struct kobj_attribute *attr,
		const char *buff, size_t count)
{
	foo_size = min(count, (size_t)FOO_SIZE_MAX);
	strncpy(foo_tmp, buff, foo_size);
	return count;
}

static struct kobj_attribute foo_attribute =
	__ATTR(foo, S_IRUGO | S_IWUSR, foo_show, foo_store);

static struct attribute *attrs[] = {
	&foo_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *kobj;

static int myinit(void)
{
	int ret;

	kobj = kobject_create_and_add("lkmc_sysfs", kernel_kobj);
	if (!kobj)
		return -ENOMEM;
	ret = sysfs_create_group(kobj, &attr_group);
	if (ret)
		kobject_put(kobj);
	return ret;
}

static void myexit(void)
{
	kobject_put(kobj);
}

module_init(myinit);
module_exit(myexit);
MODULE_LICENSE("GPL");
