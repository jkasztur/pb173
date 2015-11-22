#include <linux/module.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/stat.h>
#include <linux/mutex.h>
#include <linux/atomic.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/io.h>

int num_of_pages;
int read_count;
struct dentry *mydir;
atomic_t instance_counter;

void *main_buffer;

char hw4_buffer[128];
int hw4_num_ptr;
DEFINE_MUTEX(write_lock_mutex);

int my_open(struct inode *inode, struct file *filp)
{
	pr_info("Opening...");
	atomic_inc(&instance_counter);
	return 0;
}

int my_release(struct inode *inode, struct file *filp)
{
	pr_info("Releasing...");
	atomic_dec(&instance_counter);
	return 0;
}
ssize_t my_read(struct file *filp, char __user *buf, size_t count,
loff_t *offp)
{
	return simple_read_from_buffer(buf, count, offp, main_buffer,
		20*1024*1024);
}

/*was not in my version of kernel*/
ssize_t my_simple_write_to_buffer(void *to, size_t available, loff_t *ppos,
	const void __user *from, size_t count)
{
	loff_t pos = *ppos;
	size_t res;

	if (pos < 0)
		return -EINVAL;
	if (pos >= available || !count)
		return 0;
	if (count > available - pos)
		count = available - pos;
	res = copy_from_user(to + pos, from, count);
	if (res == count)
		return -EFAULT;
	count -= res;
	*ppos = pos + count;
	return count;
}

ssize_t my_write(struct file *filp, const char __user *buf, size_t count,
loff_t *offp)
{
	int ret = 0;

	if ((filp->f_mode & FMODE_WRITE) != 0)
		mutex_lock(&write_lock_mutex);
	ret = my_simple_write_to_buffer(main_buffer, 20*1024*1024,
		offp, buf, count);
	if ((filp->f_mode & FMODE_WRITE) != 0)
		mutex_unlock(&write_lock_mutex);
	return ret;
}

long my_unlocked_ioctl(struct file *filp, unsigned int cmd,
unsigned long arg)
{
	int ret = 0;

	switch (cmd) {
	case 100:
		ret = copy_from_user(&read_count, (int *)arg,
			sizeof(int));
		if (ret != 0) {
			pr_warn("Error when setting custom value, ret: %d\n"
				, ret);
		}
		break;
	case 200:
		ret = copy_to_user((int *)arg,
			&read_count, sizeof(int));
		if (ret != 0) {
			pr_warn("Error when getting custom value, ret: %d\n"
				, ret);
		}
		break;
	}
	return 0;
}

dev_t device_dev;
dev_t misc_dev;

const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_release,
	.unlocked_ioctl = my_unlocked_ioctl,
};

struct miscdevice my_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "my_awesome_misc_device",
	.fops = &my_fops,
};

struct cdev my_cdev;

/*--------------------------HW03--------------------------*/

ssize_t debugfs_read(struct file *file, char __user *buf, size_t count,
loff_t *offp)
{
	return simple_read_from_buffer(buf, count, offp,
		THIS_MODULE->module_core, THIS_MODULE->core_text_size);
}

const struct file_operations debugfs_fops = {
	.read = debugfs_read,
};

/*-------------------------HW04--------------------------*/
int hw4_open(struct inode *inode, struct file *filp)
{
	hw4_num_ptr = 0;
	return 0;
}
ssize_t hw4_write(struct file *filp, const char __user *buf, size_t count,
loff_t *offp)
{
	int written = 0;
	int ret;

	while (count > 0 && written < 5) {
		ret = copy_from_user(&hw4_buffer[hw4_num_ptr++], buf++,
			sizeof(char));
		if (ret != 0)
			pr_warn("Error when copying to user, ret: %d\n", ret);
		count--;
		written++;
		msleep(20);
	}
	if (count == 0)
		pr_info("Buffer: %s", hw4_buffer);
	offp += written;
	return written;
}

ssize_t hw4_read(struct file *file, char __user *buf, size_t count,
loff_t *offp)
{
	int ret;

	if (*offp >= strlen(hw4_buffer) || !count)
		return 0;
	ret = copy_to_user(buf, hw4_buffer, 128 * sizeof(char));
	if (ret != 0)
		pr_warn("didnt copy everything, ret: %d\n", ret);
	*offp += strlen(hw4_buffer);
	return 128;
}

const struct file_operations hw4_read_fops = {
	.read = hw4_read,
};

const struct file_operations hw4_write_fops = {
	.write = hw4_write,
	.open = hw4_open,
};


struct miscdevice hw4_write_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "hw4_write_misc",
	.fops = &hw4_write_fops,
};


struct miscdevice hw4_read_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "hw4_read_misc",
	.fops = &hw4_read_fops,
};

static int my_init(void)
{
	u16 value = atomic_read(&instance_counter);
	int i;

	pr_info("Adding module\n");
	main_buffer = vzalloc(20*1024*1024);
	num_of_pages = (20*1024*1024) / PAGE_SIZE;
	for (i = 0; i < num_of_pages; i++) {
		snprintf(main_buffer + (PAGE_SIZE * i), 31 * sizeof(char),
			"%p: %lx\n", main_buffer + (PAGE_SIZE * i),
			(unsigned int long)virt_to_phys
			(main_buffer + (PAGE_SIZE * i)));
		pr_info("%s", (char *)main_buffer + (PAGE_SIZE * i));
	}
	alloc_chrdev_region(&device_dev, 0, 10, "my_awesome_device");
	pr_info("Major: %d\n", MAJOR(device_dev));
	pr_info("Minor: %d\n", MINOR(device_dev));
	cdev_init(&my_cdev, &my_fops);
	cdev_add(&my_cdev, device_dev, 10);
	misc_register(&my_misc_device);
	read_count = 4;
	atomic_set(&instance_counter, 0);
	print_hex_dump_bytes("raw data: ", DUMP_PREFIX_NONE,
		THIS_MODULE->module_core, THIS_MODULE->core_text_size);
	mydir = debugfs_create_dir("my_awesome_dir", NULL);
	debugfs_create_u16("inst_counter", S_IRWXU, mydir, &value);
	debugfs_create_file("module_core_file", S_IRWXU, mydir, NULL,
		&debugfs_fops);

	misc_register(&hw4_write_misc);
	misc_register(&hw4_read_misc);
	return 0;
}

static void my_exit(void)
{
	cdev_del(&my_cdev);
	unregister_chrdev_region(device_dev, 10);
	misc_deregister(&my_misc_device);

	misc_deregister(&hw4_write_misc);
	misc_deregister(&hw4_read_misc);
	debugfs_remove_recursive(mydir);
	vfree(main_buffer);
	pr_info("Removing module\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
