/* https://cirosantilli.com/linux-kernel-module-cheat#sysfs */

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
