/* https://github.com/cirosantilli/linux-kernel-module-cheat#kernel-module-parameters */

#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <uapi/linux/stat.h> /* S_IRUSR | S_IWUSR */

static int i = 0;
static int j = 0;
module_param(i, int, S_IRUSR | S_IWUSR);
module_param(j, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(i, "my favorite int");
MODULE_PARM_DESC(j, "my second favorite int");

static struct task_struct *kthread;

static int work_func(void *data)
{
	while (!kthread_should_stop()) {
		pr_info("%d %d\n", i, j);
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
MODULE_LICENSE("GPL");
