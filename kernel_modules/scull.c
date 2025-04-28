/* https://cirosantilli.com/linux-kernel-module-cheat#scull */

/* Derived from:
 * https://github.com/martinezjavier/ldd3/tree/30f801cd0157e8dfb41193f471dc00d8ca10239f/scull
 */

/*
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 * $Id: scull.h,v 1.15 2004/11/04 17:51:18 rubini Exp $
 */

#define INCLUDE_VERMAGIC
#include <asm/atomic.h>
#include <asm/uaccess.h>
#include <linux/build-salt.h>
#include <linux/cdev.h>
#include <linux/compiler.h>
#include <linux/cred.h> /* current_uid(), current_euid() */
#include <linux/elfnote-lto.h>
#include <linux/errno.h> /* error codes */
#include <linux/export-internal.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */
#include <linux/kernel.h> /* printk() */
#include <linux/list.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/seq_file.h>
#include <linux/slab.h> /* kmalloc() */
#include <linux/tty.h>
#include <linux/types.h> /* size_t */
#include <linux/uaccess.h> /* copy_*_user */
#include <linux/vermagic.h>
#include <linux/version.h>

/* Liner kernel version dependant stuff. */

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
#define access_ok_wrapper(type,arg,cmd) \
	access_ok(type, arg, cmd)
#else
#define access_ok_wrapper(type,arg,cmd) \
	access_ok(arg, cmd)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 6, 0)
#define proc_ops_wrapper(fops, newname) (fops)
#else
#define proc_ops_wrapper(fops, newname)                                         \
({                                                                              \
        static struct proc_ops newname;                                         \
                                                                                \
        newname.proc_open = (fops)->open;                                       \
        newname.proc_read = (fops)->read;                                       \
        newname.proc_write = (fops)->write;                                     \
        newname.proc_release = (fops)->release;                                 \
        newname.proc_poll = (fops)->poll;                                       \
        newname.proc_ioctl = (fops)->unlocked_ioctl;                            \
        newname.proc_mmap = (fops)->mmap;                                       \
        newname.proc_get_unmapped_area = (fops)->get_unmapped_area;             \
        newname.proc_lseek = (fops)->llseek;                                    \
        __add_proc_ops_compat_ioctl(&newname, fops);                            \
        &newname;                                                               \
})
#endif

/* Config dependant stuff. */

#ifdef CONFIG_COMPAT
#define __add_proc_ops_compat_ioctl(pops, fops)                                 \
	(pops)->proc_compat_ioctl = (fops)->compat_ioctl
#else
#define __add_proc_ops_compat_ioctl(pops, fops)
#endif

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

/*
 * Macros to help debugging
 */

#undef PDEBUG             /* undef it, just in case */
#ifdef SCULL_DEBUG
#  ifdef __KERNEL__
	/* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "scull: " fmt, ## args)
#  else
	/* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */

#ifndef SCULL_MAJOR
#define SCULL_MAJOR 0   /* dynamic major by default */
#endif

#ifndef SCULL_NR_DEVS
#define SCULL_NR_DEVS 4    /* scull0 through scull3 */
#endif

#ifndef SCULL_P_NR_DEVS
#define SCULL_P_NR_DEVS 4  /* scullpipe0 through scullpipe3 */
#endif

/*
 * The bare device is a variable-length region of memory.
 * Use a linked list of indirect blocks.
 *
 * "scull_dev->data" points to an array of pointers, each
 * pointer refers to a memory area of SCULL_QUANTUM bytes.
 *
 * The array (quantum-set) is SCULL_QSET long.
 */
#ifndef SCULL_QUANTUM
#define SCULL_QUANTUM 4000
#endif

#ifndef SCULL_QSET
#define SCULL_QSET    1000
#endif

/*
 * The pipe device is a simple circular buffer. Here its default size
 */
#ifndef SCULL_P_BUFFER
#define SCULL_P_BUFFER 4000
#endif

/*
 * Representation of scull quantum sets.
 */
struct scull_qset {
	void **data;
	struct scull_qset *next;
};

struct scull_dev {
	struct scull_qset *data;  /* Pointer to first quantum set */
	int quantum;              /* the current quantum size */
	int qset;                 /* the current array size */
	unsigned long size;       /* amount of data stored here */
	unsigned int access_key;  /* used by sculluid and scullpriv */
	struct mutex lock;     /* mutual exclusion semaphore     */
	struct cdev cdev;	  /* Char device structure		*/
};

/*
 * Split minors in two parts
 */
#define TYPE(minor)	(((minor) >> 4) & 0xf)	/* high nibble */
#define NUM(minor)	((minor) & 0xf)		/* low  nibble */

/*
 * The different configurable parameters
 */
extern int scull_major;     /* main.c */
extern int scull_nr_devs;
extern int scull_quantum;
extern int scull_qset;

extern int scull_p_buffer;	/* pipe.c */

/*
 * Prototypes for shared functions
 */

int     scull_p_init(dev_t dev);
void    scull_p_cleanup(void);
int     scull_access_init(dev_t dev);
void    scull_access_cleanup(void);

int     scull_trim(struct scull_dev *dev);

ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
		   loff_t *f_pos);
ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
		    loff_t *f_pos);
loff_t  scull_llseek(struct file *filp, loff_t off, int whence);
long     scull_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

/*
 * Ioctl definitions
 */

/* Use 'k' as magic number */
#define SCULL_IOC_MAGIC  'k'
/* Please use a different 8-bit number in your code */

#define SCULL_IOCRESET    _IO(SCULL_IOC_MAGIC, 0)

/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 * X means "eXchange": switch G and S atomically
 * H means "sHift": switch T and Q atomically
 */
#define SCULL_IOCSQUANTUM _IOW(SCULL_IOC_MAGIC,  1, int)
#define SCULL_IOCSQSET    _IOW(SCULL_IOC_MAGIC,  2, int)
#define SCULL_IOCTQUANTUM _IO(SCULL_IOC_MAGIC,   3)
#define SCULL_IOCTQSET    _IO(SCULL_IOC_MAGIC,   4)
#define SCULL_IOCGQUANTUM _IOR(SCULL_IOC_MAGIC,  5, int)
#define SCULL_IOCGQSET    _IOR(SCULL_IOC_MAGIC,  6, int)
#define SCULL_IOCQQUANTUM _IO(SCULL_IOC_MAGIC,   7)
#define SCULL_IOCQQSET    _IO(SCULL_IOC_MAGIC,   8)
#define SCULL_IOCXQUANTUM _IOWR(SCULL_IOC_MAGIC, 9, int)
#define SCULL_IOCXQSET    _IOWR(SCULL_IOC_MAGIC,10, int)
#define SCULL_IOCHQUANTUM _IO(SCULL_IOC_MAGIC,  11)
#define SCULL_IOCHQSET    _IO(SCULL_IOC_MAGIC,  12)

/*
 * The other entities only have "Tell" and "Query", because they're
 * not printed in the book, and there's no need to have all six.
 * (The previous stuff was only there to show different ways to do it.
 */
#define SCULL_P_IOCTSIZE _IO(SCULL_IOC_MAGIC,   13)
#define SCULL_P_IOCQSIZE _IO(SCULL_IOC_MAGIC,   14)
/* ... more to come */

#define SCULL_IOC_MAXNR 14

/* FIXME: cloned devices as a use for kobjects? */
static dev_t scull_a_firstdev;  /* Where our range begins */

/*
 * These devices fall back on the main scull operations. They only
 * differ in the implementation of open() and close()
 */

/************************************************************************
 *
 * The first device is the single-open one,
 *  it has an hw structure and an open count
 */

static struct scull_dev scull_s_device;
static atomic_t scull_s_available = ATOMIC_INIT(1);

static int scull_s_open(struct inode *inode, struct file *filp)
{
	struct scull_dev *dev = &scull_s_device; /* device information */

	if (! atomic_dec_and_test (&scull_s_available)) {
		atomic_inc(&scull_s_available);
		return -EBUSY; /* already open */
	}

	/* then, everything else is copied from the bare scull device */
	if ((filp->f_flags & O_TRUNC))
		scull_trim(dev);
	filp->private_data = dev;
	return 0;          /* success */
}

static int scull_s_release(struct inode *inode, struct file *filp)
{
	atomic_inc(&scull_s_available); /* release the device */
	return 0;
}

/*
 * The other operations for the single-open device come from the bare device
 */
struct file_operations scull_sngl_fops = {
	.owner = THIS_MODULE,
	.llseek = scull_llseek,
	.read = scull_read,
	.write = scull_write,
	.unlocked_ioctl = scull_ioctl,
	.open = scull_s_open,
	.release = scull_s_release,
};

/************************************************************************
 *
 * Next, the "uid" device. It can be opened multiple times by the
 * same user, but access is denied to other users if the device is open
 */

static struct scull_dev scull_u_device;
static int scull_u_count;	/* initialized to 0 by default */
static uid_t scull_u_owner;	/* initialized to 0 by default */
static DEFINE_SPINLOCK(scull_u_lock);

static int scull_u_open(struct inode *inode, struct file *filp)
{
	struct scull_dev *dev = &scull_u_device; /* device information */

	spin_lock(&scull_u_lock);
	if (scull_u_count && 
			(scull_u_owner != current_uid().val) &&  /* allow user */
			(scull_u_owner != current_euid().val) && /* allow whoever did su */
			!capable(CAP_DAC_OVERRIDE)) { /* still allow root */
		spin_unlock(&scull_u_lock);
		return -EBUSY;   /* -EPERM would confuse the user */
	}

	if (scull_u_count == 0)
		scull_u_owner = current_uid().val; /* grab it */

	scull_u_count++;
	spin_unlock(&scull_u_lock);

/* then, everything else is copied from the bare scull device */

	if ((filp->f_flags & O_TRUNC))
		scull_trim(dev);
	filp->private_data = dev;
	return 0;          /* success */
}

static int scull_u_release(struct inode *inode, struct file *filp)
{
	spin_lock(&scull_u_lock);
	scull_u_count--; /* nothing else */
	spin_unlock(&scull_u_lock);
	return 0;
}

/*
 * The other operations for the device come from the bare device
 */
struct file_operations scull_user_fops = {
	.owner =      THIS_MODULE,
	.llseek =     scull_llseek,
	.read =       scull_read,
	.write =      scull_write,
	.unlocked_ioctl = scull_ioctl,
	.open =       scull_u_open,
	.release =    scull_u_release,
};

/************************************************************************
 *
 * Next, the device with blocking-open based on uid
 */

static struct scull_dev scull_w_device;
static int scull_w_count;	/* initialized to 0 by default */
static uid_t scull_w_owner;	/* initialized to 0 by default */
static DECLARE_WAIT_QUEUE_HEAD(scull_w_wait);
static DEFINE_SPINLOCK(scull_w_lock);

static inline int scull_w_available(void)
{
	return scull_w_count == 0 ||
		scull_w_owner == current_uid().val ||
		scull_w_owner == current_euid().val ||
		capable(CAP_DAC_OVERRIDE);
}

static int scull_w_open(struct inode *inode, struct file *filp)
{
	struct scull_dev *dev = &scull_w_device; /* device information */

	spin_lock(&scull_w_lock);
	while (! scull_w_available()) {
		spin_unlock(&scull_w_lock);
		if (filp->f_flags & O_NONBLOCK) return -EAGAIN;
		if (wait_event_interruptible (scull_w_wait, scull_w_available()))
			return -ERESTARTSYS; /* tell the fs layer to handle it */
		spin_lock(&scull_w_lock);
	}
	if (scull_w_count == 0)
		scull_w_owner = current_uid().val; /* grab it */
	scull_w_count++;
	spin_unlock(&scull_w_lock);

	/* then, everything else is copied from the bare scull device */
	if ((filp->f_flags & O_TRUNC))
		scull_trim(dev);
	filp->private_data = dev;
	return 0;          /* success */
}

static int scull_w_release(struct inode *inode, struct file *filp)
{
	int temp;

	spin_lock(&scull_w_lock);
	scull_w_count--;
	temp = scull_w_count;
	spin_unlock(&scull_w_lock);

	if (temp == 0)
		wake_up_interruptible_sync(&scull_w_wait); /* awake other uid's */
	return 0;
}

/*
 * The other operations for the device come from the bare device
 */
struct file_operations scull_wusr_fops = {
	.owner =      THIS_MODULE,
	.llseek =     scull_llseek,
	.read =       scull_read,
	.write =      scull_write,
	.unlocked_ioctl = scull_ioctl,
	.open =       scull_w_open,
	.release =    scull_w_release,
};

/************************************************************************
 *
 * Finally the `cloned' private device. This is trickier because it
 * involves list management, and dynamic allocation.
 */

/* The clone-specific data structure includes a key field */

struct scull_listitem {
	struct scull_dev device;
	dev_t key;
	struct list_head list;
};

/* The list of devices, and a lock to protect it */
static LIST_HEAD(scull_c_list);
static DEFINE_SPINLOCK(scull_c_lock);

/* A placeholder scull_dev which really just holds the cdev stuff. */
static struct scull_dev scull_c_device;   

/* Look for a device or create one if missing */
static struct scull_dev *scull_c_lookfor_device(dev_t key)
{
	struct scull_listitem *lptr;

	list_for_each_entry(lptr, &scull_c_list, list) {
		if (lptr->key == key)
			return &(lptr->device);
	}

	/* not found */
	lptr = kmalloc(sizeof(struct scull_listitem), GFP_KERNEL);
	if (!lptr)
		return NULL;

	/* initialize the device */
	memset(lptr, 0, sizeof(struct scull_listitem));
	lptr->key = key;
	scull_trim(&(lptr->device)); /* initialize it */
	mutex_init(&lptr->device.lock);

	/* place it in the list */
	list_add(&lptr->list, &scull_c_list);

	return &(lptr->device);
}

static int scull_c_open(struct inode *inode, struct file *filp)
{
	struct scull_dev *dev;
	dev_t key;
 
	if (!current->signal->tty) { 
		PDEBUG("Process \"%s\" has no ctl tty\n", current->comm);
		return -EINVAL;
	}
	key = tty_devnum(current->signal->tty);

	/* look for a scullc device in the list */
	spin_lock(&scull_c_lock);
	dev = scull_c_lookfor_device(key);
	spin_unlock(&scull_c_lock);

	if (!dev)
		return -ENOMEM;

	/* then, everything else is copied from the bare scull device */
	if ((filp->f_flags & O_TRUNC))
		scull_trim(dev);
	filp->private_data = dev;
	return 0;          /* success */
}

static int scull_c_release(struct inode *inode, struct file *filp)
{
	/*
	 * Nothing to do, because the device is persistent.
	 * A `real' cloned device should be freed on last close
	 */
	return 0;
}

/*
 * The other operations for the device come from the bare device
 */
struct file_operations scull_priv_fops = {
	.owner =    THIS_MODULE,
	.llseek =   scull_llseek,
	.read =     scull_read,
	.write =    scull_write,
	.unlocked_ioctl = scull_ioctl,
	.open =     scull_c_open,
	.release =  scull_c_release,
};

/************************************************************************
 *
 * And the init and cleanup functions come last
 */

static struct scull_adev_info {
	char *name;
	struct scull_dev *sculldev;
	struct file_operations *fops;
} scull_access_devs[] = {
	{ "scullsingle", &scull_s_device, &scull_sngl_fops },
	{ "sculluid", &scull_u_device, &scull_user_fops },
	{ "scullwuid", &scull_w_device, &scull_wusr_fops },
	{ "scullpriv", &scull_c_device, &scull_priv_fops }
};
#define SCULL_N_ADEVS 4

/*
 * Set up a single device.
 */
static void scull_access_setup (dev_t devno, struct scull_adev_info *devinfo)
{
	struct scull_dev *dev = devinfo->sculldev;
	int err;

	/* Initialize the device structure */
	dev->quantum = scull_quantum;
	dev->qset = scull_qset;
	mutex_init(&dev->lock);

	/* Do the cdev stuff. */
	cdev_init(&dev->cdev, devinfo->fops);
	kobject_set_name(&dev->cdev.kobj, devinfo->name);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err) {
		printk(KERN_NOTICE "Error %d adding %s\n", err, devinfo->name);
		kobject_put(&dev->cdev.kobj);
	} else
		printk(KERN_NOTICE "%s registered at %x\n", devinfo->name, devno);
}

int scull_access_init(dev_t firstdev)
{
	int result, i;

	/* Get our number space */
	result = register_chrdev_region (firstdev, SCULL_N_ADEVS, "sculla");
	if (result < 0) {
		printk(KERN_WARNING "sculla: device number registration failed\n");
		return 0;
	}
	scull_a_firstdev = firstdev;

	/* Set up each device. */
	for (i = 0; i < SCULL_N_ADEVS; i++)
		scull_access_setup (firstdev + i, scull_access_devs + i);
	return SCULL_N_ADEVS;
}

/*
 * This is called by cleanup_module or on failure.
 * It is required to never fail, even if nothing was initialized first
 */
void scull_access_cleanup(void)
{
	struct scull_listitem *lptr, *next;
	int i;

	/* Clean up the static devs */
	for (i = 0; i < SCULL_N_ADEVS; i++) {
		struct scull_dev *dev = scull_access_devs[i].sculldev;
		cdev_del(&dev->cdev);
		scull_trim(scull_access_devs[i].sculldev);
	}

	/* And all the cloned devices */
	list_for_each_entry_safe(lptr, next, &scull_c_list, list) {
		list_del(&lptr->list);
		scull_trim(&(lptr->device));
		kfree(lptr);
	}

	/* Free up our number space */
	unregister_chrdev_region(scull_a_firstdev, SCULL_N_ADEVS);
	return;
}

/*
 * Our parameters which can be set at load time.
 */
int scull_major =   SCULL_MAJOR;
int scull_minor =   0;
int scull_nr_devs = SCULL_NR_DEVS;	/* number of bare scull devices */
int scull_quantum = SCULL_QUANTUM;
int scull_qset =    SCULL_QSET;
module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_quantum, int, S_IRUGO);
module_param(scull_qset, int, S_IRUGO);

struct scull_dev *scull_devices;	/* allocated in scull_init_module */

/*
 * Empty out the scull device; must be called with the device
 * semaphore held.
 */
int scull_trim(struct scull_dev *dev)
{
	struct scull_qset *next, *dptr;
	int qset = dev->qset;   /* "dev" is not-null */
	int i;

	for (dptr = dev->data; dptr; dptr = next) { /* all the list items */
		if (dptr->data) {
			for (i = 0; i < qset; i++)
				kfree(dptr->data[i]);
			kfree(dptr->data);
			dptr->data = NULL;
		}
		next = dptr->next;
		kfree(dptr);
	}
	dev->size = 0;
	dev->quantum = scull_quantum;
	dev->qset = scull_qset;
	dev->data = NULL;
	return 0;
}

/*
 * Open and close
 */

int scull_open(struct inode *inode, struct file *filp);
int scull_open(struct inode *inode, struct file *filp)
{
	struct scull_dev *dev; /* device information */

	dev = container_of(inode->i_cdev, struct scull_dev, cdev);
	filp->private_data = dev; /* for other methods */

	/* Trim to 0 the length of the device was open with truncation */
	if ((filp->f_flags & O_TRUNC)) {
		pr_info("write O_TRUNC\n");
		if (mutex_lock_interruptible(&dev->lock))
			return -ERESTARTSYS;
		scull_trim(dev); /* ignore errors */
		mutex_unlock(&dev->lock);
	}
	return 0;          /* success */
}

/*
 * Follow the list
 */
struct scull_qset *scull_follow(struct scull_dev *dev, int n);
struct scull_qset *scull_follow(struct scull_dev *dev, int n)
{
	struct scull_qset *qs = dev->data;

	/* Allocate first qset explicitly if need be */
	if (! qs) {
		qs = dev->data = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
		if (qs == NULL)
			return NULL;  /* Never mind */
		memset(qs, 0, sizeof(struct scull_qset));
	}

	/* Then follow the list */
	while (n--) {
		if (!qs->next) {
			qs->next = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
			if (qs->next == NULL)
				return NULL;  /* Never mind */
			memset(qs->next, 0, sizeof(struct scull_qset));
		}
		qs = qs->next;
		continue;
	}
	return qs;
}

int scull_release(struct inode *inode, struct file *filp);
int scull_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/*
 * Data management: read and write
 */

ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos);
ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos)
{
	struct scull_dev *dev = filp->private_data; 
	struct scull_qset *dptr;	/* the first listitem */
	int quantum = dev->quantum;
	int qset = dev->qset;
	int itemsize = quantum * qset; /* how many bytes in the listitem */
	int item, s_pos, q_pos, rest;
	ssize_t retval = 0;

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;
	if (*f_pos >= dev->size)
		goto out;
	if (*f_pos + count > dev->size)
		count = dev->size - *f_pos;

	/* find listitem, qset index, and offset in the quantum */
	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum;
	q_pos = rest % quantum;

	/* follow the list up to the right position (defined elsewhere) */
	dptr = scull_follow(dev, item);

	if (dptr == NULL || !dptr->data || ! dptr->data[s_pos])
		goto out; /* don't fill holes */

	/* read only up to the end of this quantum */
	if (count > quantum - q_pos)
		count = quantum - q_pos;

	if (copy_to_user(buf, dptr->data[s_pos] + q_pos, count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

  out:
	mutex_unlock(&dev->lock);
	return retval;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
		loff_t *f_pos);
ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
		loff_t *f_pos)
{
	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum;
	int qset = dev->qset;
	int itemsize = quantum * qset;
	int item, s_pos, q_pos, rest;
	ssize_t retval = -ENOMEM; /* value used in "goto out" statements */

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;

	/* find listitem, qset index and offset in the quantum */
	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum;
	q_pos = rest % quantum;

	/* follow the list up to the right position */
	dptr = scull_follow(dev, item);
	if (dptr == NULL)
		goto out;
	if (!dptr->data) {
		dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
		if (!dptr->data)
			goto out;
		memset(dptr->data, 0, qset * sizeof(char *));
	}
	if (!dptr->data[s_pos]) {
		dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
		if (!dptr->data[s_pos])
			goto out;
	}
	/* write only up to the end of this quantum */
	if (count > quantum - q_pos)
		count = quantum - q_pos;

	if (copy_from_user(dptr->data[s_pos]+q_pos, buf, count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

	/* update the size */
	if (dev->size < *f_pos)
		dev->size = *f_pos;

  out:
	mutex_unlock(&dev->lock);
	return retval;
}

/*
 * The ioctl() implementation
 */

long scull_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
long scull_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{

	int err = 0, tmp;
	int retval = 0;

	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 */
	if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;

	/*
	 * the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 */
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok_wrapper(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok_wrapper(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err) return -EFAULT;

	switch(cmd) {

	  case SCULL_IOCRESET:
		scull_quantum = SCULL_QUANTUM;
		scull_qset = SCULL_QSET;
		break;

	  case SCULL_IOCSQUANTUM: /* Set: arg points to the value */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		retval = __get_user(scull_quantum, (int __user *)arg);
		break;

	  case SCULL_IOCTQUANTUM: /* Tell: arg is the value */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		scull_quantum = arg;
		break;

	  case SCULL_IOCGQUANTUM: /* Get: arg is pointer to result */
		retval = __put_user(scull_quantum, (int __user *)arg);
		break;

	  case SCULL_IOCQQUANTUM: /* Query: return it (it's positive) */
		return scull_quantum;

	  case SCULL_IOCXQUANTUM: /* eXchange: use arg as pointer */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_quantum;
		retval = __get_user(scull_quantum, (int __user *)arg);
		if (retval == 0)
			retval = __put_user(tmp, (int __user *)arg);
		break;

	  case SCULL_IOCHQUANTUM: /* sHift: like Tell + Query */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_quantum;
		scull_quantum = arg;
		return tmp;

	  case SCULL_IOCSQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		retval = __get_user(scull_qset, (int __user *)arg);
		break;

	  case SCULL_IOCTQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		scull_qset = arg;
		break;

	  case SCULL_IOCGQSET:
		retval = __put_user(scull_qset, (int __user *)arg);
		break;

	  case SCULL_IOCQQSET:
		return scull_qset;

	  case SCULL_IOCXQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_qset;
		retval = __get_user(scull_qset, (int __user *)arg);
		if (retval == 0)
			retval = put_user(tmp, (int __user *)arg);
		break;

	  case SCULL_IOCHQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_qset;
		scull_qset = arg;
		return tmp;

	/*
	 * The following two change the buffer size for scullpipe.
	 * The scullpipe device uses this same ioctl method, just to
	 * write less code. Actually, it's the same driver, isn't it?
	 */

	  case SCULL_P_IOCTSIZE:
		scull_p_buffer = arg;
		break;

	  case SCULL_P_IOCQSIZE:
		return scull_p_buffer;


	  default:  /* redundant, as cmd was checked against MAXNR */
		return -ENOTTY;
	}
	return retval;
}

/*
 * The "extended" operations -- only seek
 */

loff_t scull_llseek(struct file *filp, loff_t off, int whence);
loff_t scull_llseek(struct file *filp, loff_t off, int whence)
{
	struct scull_dev *dev = filp->private_data;
	loff_t newpos;

	switch(whence) {
	  case 0: /* SEEK_SET */
		newpos = off;
		break;

	  case 1: /* SEEK_CUR */
		newpos = filp->f_pos + off;
		break;

	  case 2: /* SEEK_END */
		newpos = dev->size + off;
		break;

	  default: /* can't happen */
		return -EINVAL;
	}
	if (newpos < 0) return -EINVAL;
	filp->f_pos = newpos;
	return newpos;
}

struct file_operations scull_fops = {
	.owner =    THIS_MODULE,
	.llseek =   scull_llseek,
	.read =     scull_read,
	.write =    scull_write,
	.unlocked_ioctl = scull_ioctl,
	.open =     scull_open,
	.release =  scull_release,
};

#ifdef SCULL_DEBUG /* use proc only if debugging */
/*
 * The proc filesystem: function to read and entry
 */

int scull_read_procmem(struct seq_file *s, void *v)
{
	int i, j;
	int limit = s->size - 80; /* Don't print more than this */

	for (i = 0; i < scull_nr_devs && s->count <= limit; i++) {
		struct scull_dev *d = &scull_devices[i];
		struct scull_qset *qs = d->data;
		if (mutex_lock_interruptible(&d->lock))
			return -ERESTARTSYS;
		seq_printf(s,"\nDevice %i: qset %i, q %i, sz %li\n",
			     i, d->qset, d->quantum, d->size);
		for (; qs && s->count <= limit; qs = qs->next) { /* scan the list */
			seq_printf(s, "  item at %p, qset at %p\n",
				     qs, qs->data);
			if (qs->data && !qs->next) /* dump only the last item */
				for (j = 0; j < d->qset; j++) {
					if (qs->data[j])
						seq_printf(s, "    % 4i: %8p\n",
							     j, qs->data[j]);
				}
		}
		mutex_unlock(&scull_devices[i].lock);
	}
	return 0;
}



/*
 * Here are our sequence iteration methods.  Our "position" is
 * simply the device number.
 */
static void *scull_seq_start(struct seq_file *s, loff_t *pos)
{
	if (*pos >= scull_nr_devs)
		return NULL;   /* No more to read */
	return scull_devices + *pos;
}

static void *scull_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	(*pos)++;
	if (*pos >= scull_nr_devs)
		return NULL;
	return scull_devices + *pos;
}

static void scull_seq_stop(struct seq_file *s, void *v)
{
	/* Actually, there's nothing to do here */
}

static int scull_seq_show(struct seq_file *s, void *v)
{
	struct scull_dev *dev = (struct scull_dev *) v;
	struct scull_qset *d;
	int i;

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;
	seq_printf(s, "\nDevice %i: qset %i, q %i, sz %li\n",
			(int) (dev - scull_devices), dev->qset,
			dev->quantum, dev->size);
	for (d = dev->data; d; d = d->next) { /* scan the list */
		seq_printf(s, "  item at %p, qset at %p\n", d, d->data);
		if (d->data && !d->next) /* dump only the last item */
			for (i = 0; i < dev->qset; i++) {
				if (d->data[i])
					seq_printf(s, "    % 4i: %8p\n",
							i, d->data[i]);
			}
	}
	mutex_unlock(&dev->lock);
	return 0;
}
	
/*
 * Tie the sequence operators up.
 */
static struct seq_operations scull_seq_ops = {
	.start = scull_seq_start,
	.next  = scull_seq_next,
	.stop  = scull_seq_stop,
	.show  = scull_seq_show
};

/*
 * Now to implement the /proc files we need only make an open
 * method which sets up the sequence operators.
 */
static int scullmem_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, scull_read_procmem, NULL);
}

static int scullseq_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &scull_seq_ops);
}

/*
 * Create a set of file operations for our proc files.
 */
static struct file_operations scullmem_proc_ops = {
	.owner   = THIS_MODULE,
	.open    = scullmem_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static struct file_operations scullseq_proc_ops = {
	.owner   = THIS_MODULE,
	.open    = scullseq_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release
};
	

/*
 * Actually create (and remove) the /proc file(s).
 */

static void scull_create_proc(void)
{
	proc_create_data("scullmem", 0 /* default mode */,
			NULL /* parent dir */, proc_ops_wrapper(&scullmem_proc_ops, scullmem_pops),
			NULL /* client data */);
	proc_create("scullseq", 0, NULL, proc_ops_wrapper(&scullseq_proc_ops, scullseq_pops));
}

static void scull_remove_proc(void)
{
	/* no problem if it was not registered */
	remove_proc_entry("scullmem", NULL /* parent dir */);
	remove_proc_entry("scullseq", NULL);
}

#endif /* SCULL_DEBUG */

/*
 * Finally, the module stuff
 */

/*
 * The cleanup function is used to handle initialization failures as well.
 * Thefore, it must be careful to work correctly even if some of the items
 * have not been initialized
 */
void scull_cleanup_module(void);
void scull_cleanup_module(void)
{
	int i;
	dev_t devno = MKDEV(scull_major, scull_minor);

	/* Get rid of our char dev entries */
	if (scull_devices) {
		for (i = 0; i < scull_nr_devs; i++) {
			scull_trim(scull_devices + i);
			cdev_del(&scull_devices[i].cdev);
		}
		kfree(scull_devices);
	}

#ifdef SCULL_DEBUG /* use proc only if debugging */
	scull_remove_proc();
#endif

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, scull_nr_devs);

	/* and call the cleanup functions for friend devices */
	scull_p_cleanup();
	scull_access_cleanup();

}

/*
 * Set up the char_dev structure for this device.
 */
static void scull_setup_cdev(struct scull_dev *dev, int index)
{
	int err, devno = MKDEV(scull_major, scull_minor + index);

	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}

int scull_init_module(void);
int scull_init_module(void)
{
	int result, i;
	dev_t dev = 0;

	/*
	 * Get a range of minor numbers to work with, asking for a dynamic
	 * major unless directed otherwise at load time.
	 */
	if (scull_major) {
		dev = MKDEV(scull_major, scull_minor);
		result = register_chrdev_region(dev, scull_nr_devs, "scull");
	} else {
		result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs,
				"scull");
		scull_major = MAJOR(dev);
	}
	if (result < 0) {
		printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
		return result;
	}

	/* 
	 * allocate the devices -- we can't have them static, as the number
	 * can be specified at load time
	 */
	scull_devices = kmalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
	if (!scull_devices) {
		result = -ENOMEM;
		goto fail;  /* Make this more graceful */
	}
	memset(scull_devices, 0, scull_nr_devs * sizeof(struct scull_dev));

	/* Initialize each device. */
	for (i = 0; i < scull_nr_devs; i++) {
		scull_devices[i].quantum = scull_quantum;
		scull_devices[i].qset = scull_qset;
		mutex_init(&scull_devices[i].lock);
		scull_setup_cdev(&scull_devices[i], i);
	}

	/* At this point call the init function for any friend device */
	dev = MKDEV(scull_major, scull_minor + scull_nr_devs);
	dev += scull_p_init(dev);
	dev += scull_access_init(dev);

#ifdef SCULL_DEBUG /* only when debugging */
	scull_create_proc();
#endif

	return 0; /* succeed */

  fail:
	scull_cleanup_module();
	return result;
}

module_init(scull_init_module);
module_exit(scull_cleanup_module);
/*
 * pipe.c -- fifo driver for scull
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 */

struct scull_pipe {
	wait_queue_head_t inq, outq;       /* read and write queues */
	char *buffer, *end;                /* begin of buf, end of buf */
	int buffersize;                    /* used in pointer arithmetic */
	char *rp, *wp;                     /* where to read, where to write */
	int nreaders, nwriters;            /* number of openings for r/w */
	struct fasync_struct *async_queue; /* asynchronous readers */
	struct mutex lock;                 /* mutual exclusion mutex */
	struct cdev cdev;                  /* Char device structure */
};

/* parameters */
static int scull_p_nr_devs = SCULL_P_NR_DEVS;	/* number of pipe devices */
int scull_p_buffer =  SCULL_P_BUFFER;	/* buffer size */
dev_t scull_p_devno;			/* Our first device number */

module_param(scull_p_nr_devs, int, 0);	/* FIXME check perms */
module_param(scull_p_buffer, int, 0);

static struct scull_pipe *scull_p_devices;

static int scull_p_fasync(int fd, struct file *filp, int mode);

static int spacefree(struct scull_pipe *dev);

/*
 * Open and close
 */

static int scull_p_open(struct inode *inode, struct file *filp)
{
	struct scull_pipe *dev;

	dev = container_of(inode->i_cdev, struct scull_pipe, cdev);
	filp->private_data = dev;

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;
	if (!dev->buffer) {
		/* allocate the buffer */
		dev->buffer = kmalloc(scull_p_buffer, GFP_KERNEL);
		if (!dev->buffer) {
			mutex_unlock(&dev->lock);
			return -ENOMEM;
		}
	}
	dev->buffersize = scull_p_buffer;
	dev->end = dev->buffer + dev->buffersize;
	dev->rp = dev->wp = dev->buffer; /* rd and wr from the beginning */

	/* use f_mode,not  f_flags: it's cleaner (fs/open.c tells why) */
	if (filp->f_mode & FMODE_READ)
		dev->nreaders++;
	if (filp->f_mode & FMODE_WRITE)
		dev->nwriters++;
	mutex_unlock(&dev->lock);

	return nonseekable_open(inode, filp);
}

static int scull_p_release(struct inode *inode, struct file *filp)
{
	struct scull_pipe *dev = filp->private_data;

	/* remove this filp from the asynchronously notified filp's */
	scull_p_fasync(-1, filp, 0);
	mutex_lock(&dev->lock);
	if (filp->f_mode & FMODE_READ)
		dev->nreaders--;
	if (filp->f_mode & FMODE_WRITE)
		dev->nwriters--;
	if (dev->nreaders + dev->nwriters == 0) {
		kfree(dev->buffer);
		dev->buffer = NULL; /* the other fields are not checked on open */
	}
	mutex_unlock(&dev->lock);
	return 0;
}

/*
 * Data management: read and write
 */
static ssize_t scull_p_read (struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos)
{
	struct scull_pipe *dev = filp->private_data;

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;

	while (dev->rp == dev->wp) { /* nothing to read */
		mutex_unlock(&dev->lock); /* release the lock */
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;
		PDEBUG("\"%s\" reading: going to sleep\n", current->comm);
		if (wait_event_interruptible(dev->inq, (dev->rp != dev->wp)))
			return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
		/* otherwise loop, but first reacquire the lock */
		if (mutex_lock_interruptible(&dev->lock))
			return -ERESTARTSYS;
	}
	/* ok, data is there, return something */
	if (dev->wp > dev->rp)
		count = min(count, (size_t)(dev->wp - dev->rp));
	else /* the write pointer has wrapped, return data up to dev->end */
		count = min(count, (size_t)(dev->end - dev->rp));
	if (copy_to_user(buf, dev->rp, count)) {
		mutex_unlock (&dev->lock);
		return -EFAULT;
	}
	dev->rp += count;
	if (dev->rp == dev->end)
		dev->rp = dev->buffer; /* wrapped */
	mutex_unlock (&dev->lock);

	/* finally, awake any writers and return */
	wake_up_interruptible(&dev->outq);
	PDEBUG("\"%s\" did read %li bytes\n",current->comm, (long)count);
	return count;
}

/* Wait for space for writing; caller must hold device semaphore.  On
 * error the semaphore will be released before returning. */
static int scull_getwritespace(struct scull_pipe *dev, struct file *filp)
{
	while (spacefree(dev) == 0) { /* full */
		DEFINE_WAIT(wait);
		
		mutex_unlock(&dev->lock);
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;
		PDEBUG("\"%s\" writing: going to sleep\n",current->comm);
		prepare_to_wait(&dev->outq, &wait, TASK_INTERRUPTIBLE);
		if (spacefree(dev) == 0)
			schedule();
		finish_wait(&dev->outq, &wait);
		if (signal_pending(current))
			return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
		if (mutex_lock_interruptible(&dev->lock))
			return -ERESTARTSYS;
	}
	return 0;
}	

/* How much space is free? */
static int spacefree(struct scull_pipe *dev)
{
	if (dev->rp == dev->wp)
		return dev->buffersize - 1;
	return ((dev->rp + dev->buffersize - dev->wp) % dev->buffersize) - 1;
}

static ssize_t scull_p_write(struct file *filp, const char __user *buf, size_t count,
		loff_t *f_pos)
{
	struct scull_pipe *dev = filp->private_data;
	int result;

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;

	/* Make sure there's space to write */
	result = scull_getwritespace(dev, filp);
	if (result)
		return result; /* scull_getwritespace called up(&dev->sem) */

	/* ok, space is there, accept something */
	count = min(count, (size_t)spacefree(dev));
	if (dev->wp >= dev->rp)
		count = min(count, (size_t)(dev->end - dev->wp)); /* to end-of-buf */
	else /* the write pointer has wrapped, fill up to rp-1 */
		count = min(count, (size_t)(dev->rp - dev->wp - 1));
	PDEBUG("Going to accept %li bytes to %p from %p\n", (long)count, dev->wp, buf);
	if (copy_from_user(dev->wp, buf, count)) {
		mutex_unlock(&dev->lock);
		return -EFAULT;
	}
	dev->wp += count;
	if (dev->wp == dev->end)
		dev->wp = dev->buffer; /* wrapped */
	mutex_unlock(&dev->lock);

	/* finally, awake any reader */
	wake_up_interruptible(&dev->inq);  /* blocked in read() and select() */

	/* and signal asynchronous readers, explained late in chapter 5 */
	if (dev->async_queue)
		kill_fasync(&dev->async_queue, SIGIO, POLL_IN);
	PDEBUG("\"%s\" did write %li bytes\n",current->comm, (long)count);
	return count;
}

static unsigned int scull_p_poll(struct file *filp, poll_table *wait)
{
	struct scull_pipe *dev = filp->private_data;
	unsigned int mask = 0;

	/*
	 * The buffer is circular; it is considered full
	 * if "wp" is right behind "rp" and empty if the
	 * two are equal.
	 */
	mutex_lock(&dev->lock);
	poll_wait(filp, &dev->inq,  wait);
	poll_wait(filp, &dev->outq, wait);
	if (dev->rp != dev->wp)
		mask |= POLLIN | POLLRDNORM;	/* readable */
	if (spacefree(dev))
		mask |= POLLOUT | POLLWRNORM;	/* writable */
	mutex_unlock(&dev->lock);
	return mask;
}

static int scull_p_fasync(int fd, struct file *filp, int mode)
{
	struct scull_pipe *dev = filp->private_data;

	return fasync_helper(fd, filp, mode, &dev->async_queue);
}

/* FIXME this should use seq_file */
#ifdef SCULL_DEBUG

static int scull_read_p_mem(struct seq_file *s, void *v)
{
	int i;
	struct scull_pipe *p;

#define LIMIT (PAGE_SIZE-200)        /* don't print any more after this size */
	seq_printf(s, "Default buffersize is %i\n", scull_p_buffer);
	for(i = 0; i<scull_p_nr_devs && s->count <= LIMIT; i++) {
		p = &scull_p_devices[i];
		if (mutex_lock_interruptible(&p->lock))
			return -ERESTARTSYS;
		seq_printf(s, "\nDevice %i: %p\n", i, p);
/*		seq_printf(s, "   Queues: %p %p\n", p->inq, p->outq);*/
		seq_printf(s, "   Buffer: %p to %p (%i bytes)\n", p->buffer, p->end, p->buffersize);
		seq_printf(s, "   rp %p   wp %p\n", p->rp, p->wp);
		seq_printf(s, "   readers %i   writers %i\n", p->nreaders, p->nwriters);
		mutex_unlock(&p->lock);
	}
	return 0;
}

static int scullpipe_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, scull_read_p_mem, NULL);
}

static struct file_operations scullpipe_proc_ops = {
	.owner   = THIS_MODULE,
	.open    = scullpipe_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

#endif

/*
 * The file operations for the pipe device
 * (some are overlayed with bare scull)
 */
struct file_operations scull_pipe_fops = {
	.owner =	THIS_MODULE,
	.llseek =	no_llseek,
	.read =		scull_p_read,
	.write =	scull_p_write,
	.poll =		scull_p_poll,
	.unlocked_ioctl = scull_ioctl,
	.open =		scull_p_open,
	.release =	scull_p_release,
	.fasync =	scull_p_fasync,
};

/*
 * Set up a cdev entry.
 */
static void scull_p_setup_cdev(struct scull_pipe *dev, int index)
{
	int err, devno = scull_p_devno + index;

	cdev_init(&dev->cdev, &scull_pipe_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scullpipe%d", err, index);
}

/*
 * Initialize the pipe devs; return how many we did.
 */
int scull_p_init(dev_t firstdev)
{
	int i, result;

	result = register_chrdev_region(firstdev, scull_p_nr_devs, "scullp");
	if (result < 0) {
		printk(KERN_NOTICE "Unable to get scullp region, error %d\n", result);
		return 0;
	}
	scull_p_devno = firstdev;
	scull_p_devices = kmalloc(scull_p_nr_devs * sizeof(struct scull_pipe), GFP_KERNEL);
	if (scull_p_devices == NULL) {
		unregister_chrdev_region(firstdev, scull_p_nr_devs);
		return 0;
	}
	memset(scull_p_devices, 0, scull_p_nr_devs * sizeof(struct scull_pipe));
	for (i = 0; i < scull_p_nr_devs; i++) {
		init_waitqueue_head(&(scull_p_devices[i].inq));
		init_waitqueue_head(&(scull_p_devices[i].outq));
		mutex_init(&scull_p_devices[i].lock);
		scull_p_setup_cdev(scull_p_devices + i, i);
	}
#ifdef SCULL_DEBUG
	proc_create("scullpipe", 0, NULL, proc_ops_wrapper(&scullpipe_proc_ops,scullpipe_pops));
#endif
	return scull_p_nr_devs;
}

/*
 * This is called by cleanup_module or on failure.
 * It is required to never fail, even if nothing was initialized first
 */
void scull_p_cleanup(void)
{
	int i;

#ifdef SCULL_DEBUG
	remove_proc_entry("scullpipe", NULL);
#endif

	if (!scull_p_devices)
		return; /* nothing else to release */

	for (i = 0; i < scull_p_nr_devs; i++) {
		cdev_del(&scull_p_devices[i].cdev);
		kfree(scull_p_devices[i].buffer);
	}
	kfree(scull_p_devices);
	unregister_chrdev_region(scull_p_devno, scull_p_nr_devs);
	scull_p_devices = NULL; /* pedantic */
}

BUILD_SALT;
BUILD_LTO_INFO;
MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);
MODULE_AUTHOR("Alessandro Rubini, Jonathan Corbet");
MODULE_LICENSE("Dual BSD/GPL");
