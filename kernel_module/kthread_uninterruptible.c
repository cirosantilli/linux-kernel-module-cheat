/*
Let's block the entire kernel! Yay!

Also try after dmesg -n 1 to become convinced of the full blockage.
*/

#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static struct task_struct *kthread;

static int work_func(void *data)
{
	set_current_state(TASK_UNINTERRUPTIBLE);
	int i = 0;
	while (!kthread_should_stop()) {
		printk(KERN_INFO "%d\n", i);
		i++;
		if (i == 10)
			i = 0;
	}
	return 0;
}

int init_module(void)
{
	kthread = kthread_create(work_func, NULL, "mykthread");
	wake_up_process(kthread);
	return 0;
}

void cleanup_module(void)
{
	kthread_stop(kthread);
}
