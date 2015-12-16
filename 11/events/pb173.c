#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/completion.h>
#include <linux/wait.h>
#include <linux/kthread.h>
static DEFINE_MUTEX(my_lock);
static DECLARE_COMPLETION(my_completion);
static DECLARE_WAIT_QUEUE_HEAD(my_wait);
static char str[10];
static atomic_t counter = ATOMIC_INIT(0);

static ssize_t my_read(struct file *filp, char __user *buf, size_t count,
		loff_t *off)
{
	ssize_t ret;

	if (wait_for_completion_interruptible(&my_completion)) {
		pr_info("read interrupted");
		return -ERESTARTSYS;
	}
	mutex_lock(&my_lock);
	ret = simple_read_from_buffer(buf, count, off, str, strlen(str));
	mutex_unlock(&my_lock);

	return ret;
}

static ssize_t my_write(struct file *filp, const char __user *buf, size_t count,
		loff_t *off)
{
	ssize_t ret;

	mutex_lock(&my_lock);
	ret = simple_write_to_buffer(str, sizeof(str) - 1, off, buf, count);
	if (!ret)
		str[sizeof(str) - 1] = 0;
	mutex_unlock(&my_lock);
	atomic_inc(&counter);
	wake_up(&my_wait);

	return ret;
}

static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
};

static struct miscdevice my_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &my_fops,
	.name = "my_name",
};

static void rewrite_a_to_b(void)
{
	int i = 0;

	while (i < strlen(str))	{
		if (str[i] == 'a')
			str[i] = 'b';
		i++;
	}
}

static int my_fun(void *data)
{
	while (true) {
		if (wait_event_interruptible(my_wait, strlen(str) > 0)) {
			pr_info("thread interrupted");
			return -ERESTARTSYS;
		}
		if (kthread_should_stop())
			break;
		rewrite_a_to_b();
		atomic_dec(&counter);
		complete(&my_completion);

	}
	return 0;
}

struct task_struct *my_thread;
void *my_data;
static int my_init(void)
{
	my_thread = kthread_run(my_fun, my_data, "my_thread");
	return misc_register(&my_misc);
}

static void my_exit(void)
{
	kthread_stop(my_thread);
	wake_up(&my_wait);
	misc_deregister(&my_misc);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
