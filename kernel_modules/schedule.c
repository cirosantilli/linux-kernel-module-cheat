/* https://cirosantilli.com/linux-kernel-module-cheat#schedule */

#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <uapi/linux/stat.h> /* S_IRUSR | S_IWUSR */

static int do_schedule = 1;
module_param_named(schedule, do_schedule, int,  S_IRUSR | S_IWUSR);

static struct task_struct *kthread;

static int work_func(void *data)
{
	unsigned int i = 0;
	while (!kthread_should_stop()) {
		pr_info("%u\n", i);
		i++;
		if (do_schedule)
			schedule();
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
MODULE_LICENSE("GPL");
