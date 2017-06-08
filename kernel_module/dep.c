/*
Exports the lkmc_dep which dep2.ko uses.

	insmod /dep.ko
	# dmesg => 0
	# dmesg => 0
	# dmesg => ...
	insmod /dep2.ko
	# dmesg => 1
	# dmesg => 2
	# dmesg => ...
	rmmod dep
	# Fails because dep2 uses it.
	rmmod dep2
	# Dmesg stops incrementing.
	rmmod dep

sys visibility:

	dmesg -n 1
	insmod /dep.ko
	insmod /dep2.ko
	ls -l /sys/module/dep/holders
	# => ../../dep2
	cat refcnt
	# => 1

depmod:

	grep dep "/lib/module/"*"/depmod"
	# extra/dep2.ko: extra/dep.ko
	# extra/dep.ko:
	modprobe dep
	# lsmod
	# Both dep and dep2 were loaded.

TODO: at what point does buildroot / busybox generate that file?
*/

#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

int lkmc_dep = 0;
EXPORT_SYMBOL(lkmc_dep);
static struct task_struct *kthread;

static int work_func(void *data)
{
	while (!kthread_should_stop()) {
		printk(KERN_INFO "%d\n", lkmc_dep);
		usleep_range(1000000, 1000001);
	}
	return 0;
}

static int myinit(void)
{
	kthread = kthread_create(work_func, NULL, "mykthread");
	wake_up_process(kthread);
	return 0;
}

static void myexit(void)
{
	kthread_stop(kthread);
}

module_init(myinit)
module_exit(myexit)
