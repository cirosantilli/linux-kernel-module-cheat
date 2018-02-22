/*
Print the jiffies every second.

Timers are callbacks that run when an interrupt happens, from the interrupt context itself.

Therefore they produce more accurate timing than thread scheduling, which is more complex,
but you can't do too much work inside of them.

See also:

- http://stackoverflow.com/questions/10812858/timers-in-linux-device-drivers
- https://gist.github.com/yagihiro/310149
*/

#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>

/* We would normally mark this as static and give it a more generic name.
 * But let's do it like this this time for the sake of our GDB kernel module step debugging example. */
void lkmc_timer_callback(struct timer_list *data);
static unsigned long onesec;

DEFINE_TIMER(mytimer, lkmc_timer_callback);

void lkmc_timer_callback(struct timer_list *data)
{
	pr_info("%u\n", (unsigned)jiffies);
	mod_timer(&mytimer, jiffies + onesec);
}

static int myinit(void)
{
	onesec = msecs_to_jiffies(1000);
	mod_timer(&mytimer, jiffies + onesec);
	return 0;
}

static void myexit(void)
{
	del_timer(&mytimer);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
