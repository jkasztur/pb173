#include <linux/module.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/kfifo.h>
#include <linux/slab.h>

struct kfifo my_fifo;

ssize_t my_read(struct file *filp, char __user *buf, size_t count,
loff_t *offp)
{
	char *temp;
	int size = kfifo_peek_len(&my_fifo);

	if (kfifo_is_empty(&my_fifo))
		return 0;
	if (count < size)
		size = count;
	temp = kmalloc(size, GFP_KERNEL);
	kfifo_out(&my_fifo, temp, size);
	copy_to_user(buf, temp, size);

	kfree(temp);
	return size;
}

ssize_t my_write(struct file *filp, const char __user *buf, size_t count,
loff_t *offp)
{
	char *temp;
	int size = count;

	if (count == 0 || kfifo_avail(&my_fifo) == 0)
		return 0;
	if (kfifo_avail(&my_fifo) <= count)
		size = kfifo_avail(&my_fifo);
	temp = kmalloc(size, GFP_KERNEL);
	copy_from_user(temp, buf, size);
	kfifo_in(&my_fifo, temp, size);
	kfree(temp);
	return size;
}

const struct file_operations hw6_fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
};

struct miscdevice my_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "hw6_misc",
	.fops = &hw6_fops,
};

static int my_init(void)
{

	pr_info("Adding module\n");
	kfifo_alloc(&my_fifo, 8 * 1024, GFP_KERNEL);
	misc_register(&my_misc_device);
	return 0;
}

static void my_exit(void)
{
	kfifo_free(&my_fifo);
	misc_deregister(&my_misc_device);
	pr_info("Removing module\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
