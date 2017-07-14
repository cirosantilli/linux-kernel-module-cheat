/*
Let's block the entire kernel! Yay!

kthreads only allow interrupting if you call schedule.

If you don't, they just run forever, and you have to kill the VM.

Sleep functions like usleep_range also end up calling schedule.

Test with:

	dmesg -n 1
	insmod /schedule.ko yn=[01]
	dmesg | tail

Then:

- 	yn=0:
	- `qemu -smp 1`: everything blocks!
	- `qemu -smp 2`: you can still use the board, but is it noticeably slow
- 	yn=1: all good
*/

#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <uapi/linux/stat.h> /* S_IRUSR | S_IWUSR */

static int yn = 1;
module_param(yn, int,  S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(yn, "A short integer");

static struct task_struct *kthread;

static int work_func(void *data)
{
	unsigned int i = 0;
	while (!kthread_should_stop()) {
		pr_info("%u\n", i);
		i++;
		if (yn)
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
