/* https://github.com/cirosantilli/linux-kernel-module-cheat#kthreads */

#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>

static struct task_struct *kthread1, *kthread2;

static int work_func1(void *data)
{
	int i = 0;
	while (!kthread_should_stop()) {
		pr_info("1 %d\n", i);
		usleep_range(1000000, 1000001);
		i++;
		if (i == 10)
			i = 0;
	}
	return 0;
}

static int work_func2(void *data)
{
	int i = 0;
	while (!kthread_should_stop()) {
		pr_info("2 %d\n", i);
		usleep_range(1000000, 1000001);
		i++;
		if (i == 10)
			i = 0;
	}
	return 0;
}

static int myinit(void)
{
	kthread1 = kthread_create(work_func1, NULL, "mykthread1");
	kthread2 = kthread_create(work_func2, NULL, "mykthread2");
	wake_up_process(kthread1);
	wake_up_process(kthread2);
	return 0;
}

static void myexit(void)
{
	kthread_stop(kthread1);
	kthread_stop(kthread2);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
