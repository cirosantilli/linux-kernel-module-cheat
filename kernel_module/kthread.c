/*
Kernel threads are managed exactly like userland threads.

They also have a backing task_struct, and are scheduled with the same mechanism.

See also:

- http://stackoverflow.com/questions/10177641/proper-way-of-handling-threads-in-kernel
- http://stackoverflow.com/questions/4084708/how-to-wait-for-a-linux-kernel-thread-kthreadto-exit
*/

#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>

static struct task_struct *kthread;

static int work_func(void *data)
{
	int i = 0;
	while (!kthread_should_stop()) {
		pr_info("%d\n", i);
		usleep_range(1000000, 1000001);
		i++;
		if (i == 10)
			i = 0;
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
	/* Waits for thread to return. */
	kthread_stop(kthread);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
