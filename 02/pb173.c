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

int read_count;
struct dentry *mydir;
u16 instance_counter;
char hw4_buffer[128];

int my_open(struct inode *inode, struct file *filp)
{
	pr_info("Opening..");
	instance_counter++;
	return 0;
}

int my_release(struct inode *inode, struct file *filp)
{
	pr_info("Releasing..");
	instance_counter--;
	return 0;
}

ssize_t my_read(struct file *filp, char __user *buf, size_t count,
loff_t *offp)
{
	int char_count = read_count;
	int cur_ptr = 0;
	char msg[] = "Ahoj\0";
	char temp = msg[cur_ptr];

	while (char_count > 0 && temp != '\0') {
		pr_info("%c", temp);
		cur_ptr++;
		char_count--;
		temp = msg[cur_ptr];
	}
	pr_info("\n");
	return 0;
}

ssize_t my_write(struct file *filp, const char __user *buf, size_t count,
loff_t *offp)
{
	char string[count + 1];
	int var = copy_from_user(string, buf, count);

	if (var != 0)
		return -EFAULT;
	string[count] = '\0';
	pr_info("%s\n", string);
	return count;
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
			&read_count, sizeof(int *));
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


static int my_init(void)
{
	pr_info("Adding module\n");

	alloc_chrdev_region(&device_dev, 0, 10, "my_awesome_device");
	pr_info("Major: %d\n", MAJOR(device_dev));
	pr_info("Minor: %d\n", MINOR(device_dev));
	cdev_init(&my_cdev, &my_fops);
	cdev_add(&my_cdev, device_dev, 10);
	misc_register(&my_misc_device);
	read_count = 4;

	instance_counter = 0;
	print_hex_dump_bytes("raw data: ", DUMP_PREFIX_NONE,
		THIS_MODULE->module_core, THIS_MODULE->core_text_size);
	mydir = debugfs_create_dir("my_awesome_dir", NULL);
	debugfs_create_u16("inst_counter", S_IRWXU, mydir, &instance_counter);
	debugfs_create_file("module_core_file", S_IRWXU, mydir, NULL,
		&debugfs_fops);

	return 0;
}

static void my_exit(void)
{
	cdev_del(&my_cdev);
	unregister_chrdev_region(device_dev, 10);
	misc_deregister(&my_misc_device);
	debugfs_remove_recursive(mydir);
	pr_info("Removing module\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
