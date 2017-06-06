#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

extern int lkmc_dep;
static struct task_struct *kthread;

static int work_func(void *data)
{
	while (!kthread_should_stop()) {
		usleep_range(1000000, 1000001);
		lkmc_dep++;
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
