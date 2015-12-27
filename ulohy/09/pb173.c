#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/gfp.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/mman.h>
void *temp1;
void *temp2;
void *temp3;
void *temp4;

char *buffer;
int task_mapped;

unsigned int fun(unsigned int data);

int my_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	struct page *page = 0;
	unsigned long offset;

	pr_info("myfault beginning\n");
	offset = vmf->pgoff;
	pr_info("offset: %lx", offset);
	if ((0 <= offset) && (offset < 1)) {
		if ((vma->vm_flags & (VM_WRITE | VM_READ)) != VM_READ) {
			pr_info("Bad flags\n");
			return -EINVAL;
		}
		pr_info("returning temp1\n");
		page = vmalloc_to_page(temp1);
		if (!page) {
			pr_warn("page is not set, offset is %lx\n", offset);
			return VM_FAULT_SIGBUS;
		}
	}
	if ((1 <= offset) && (offset < 2)) {
		if ((vma->vm_flags & (VM_WRITE | VM_READ)) != VM_READ) {
			pr_info("Bad flags\n");
			return -EINVAL;
		}
		pr_info("returnnig temp2\n");
		page = vmalloc_to_page(temp2);
		if (!page) {
			pr_warn("page is not set, offset is %lx\n", offset);
			return VM_FAULT_SIGBUS;
		}
	}
	if ((2 <= offset) && (offset < 3)) {
		if ((vma->vm_flags & (VM_READ | VM_WRITE))
			!= (VM_READ | VM_WRITE)) {
			pr_info("Bad flags\n");
			return -EINVAL;
		}
		pr_info("returning temp3\n");
		page = virt_to_page(temp3);
	}
	if ((3 <= offset) && (offset < 4)) {
		if ((vma->vm_flags & (VM_READ | VM_WRITE))
			!= (VM_READ | VM_WRITE)) {
			pr_info("Bad flags\n");
			return -EINVAL;
		}
		pr_info("returning temp4\n");
		page = virt_to_page(temp4);
	}
	if (!page) {
		pr_warn("page is not set, offset is %lx\n", offset);
		return VM_FAULT_SIGBUS;
	}
	get_page(page);
	vmf->page = page;
	return 0;
}

struct vm_operations_struct my_vm_ops = {
	.fault = my_fault,
};

int my_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned int return_value;
	unsigned int (*fun)(unsigned int);
	pr_info("my_mmap starting\n");
	if (task_mapped == 0) {
		remap_vmalloc_range_partial(vma, 0x800000, buffer, 10000);
		task_mapped = 1;
		pr_info("task.bin has been successfuly mapped\n");
		fun = (void *)0x800000;
		return_value = (*fun)(0);
		pr_info("function 0: %u\n", return_value);
	/*	pr_info("function 1: %u\n", (*fun)(1));
		pr_info("function 45: %u\n", (*fun)(45));
		pr_info("function 90: %u\n", (*fun)(90));
	*/
	}
	if ((0 <= vma->vm_pgoff) && (vma->vm_pgoff < 1)) {
		if ((vma->vm_flags & (VM_WRITE | VM_READ)) != VM_READ) {
			pr_info("Bad flags1\n");
			return -EINVAL;
		}
		vma->vm_ops = &my_vm_ops;
		return 0;
	}
	if ((1 <= vma->vm_pgoff) && (vma->vm_pgoff < 2)) {
		if ((vma->vm_flags & (VM_WRITE | VM_READ)) != VM_READ) {
			pr_info("Bad flags2\n");
			return -EINVAL;
		}
		vma->vm_ops = &my_vm_ops;
		return 0;
	}
	if ((2 <= vma->vm_pgoff) && (vma->vm_pgoff < 3)) {
		if ((vma->vm_flags & (VM_READ | VM_WRITE))
			!= (VM_READ | VM_WRITE)) {
			pr_info("Bad flags3\n");
			return -EINVAL;
		}
		vma->vm_ops = &my_vm_ops;
		return 0;
	}
	if ((3 <= vma->vm_pgoff) && (vma->vm_pgoff < 4)) {
		if ((vma->vm_flags & (VM_READ | VM_WRITE))
			!= (VM_READ | VM_WRITE)) {
			pr_info("Bad flags4\n");
			return -EINVAL;
		}
		vma->vm_ops = &my_vm_ops;
		return 0;
	}
	pr_info("out of range\n");
	return -EFAULT;
}

const struct file_operations hw9_fops = {
	.owner = THIS_MODULE,
	.mmap = my_mmap,
};

struct miscdevice my_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "MYDEVICE",
	.fops = &hw9_fops,
};

struct file *f;

static int my_init(void)
{
	mm_segment_t fs;

	pr_info("Adding module.\n");
	task_mapped = 0;
	temp1 = vmalloc_user(PAGE_SIZE);
	temp2 = vmalloc_user(PAGE_SIZE);
	temp3 = (void *)__get_free_page(GFP_KERNEL);
	temp4 = (void *)__get_free_page(GFP_KERNEL);
	strcpy(temp1, "aaa");
	strcpy(temp2, "bbb");
	strcpy(temp3, "ccc");
	strcpy(temp4, "ddd");
	pr_info("temp1: %p\n", temp1);
	pr_info("temp2: %p\n", temp2);
	pr_info("temp3: %p\n", temp3);
	pr_info("temp4: %p\n", temp4);

	buffer = vmalloc(10000);
	memset(buffer, 0, 10000);
	f = filp_open("/home/vyvoj/repos/pb173/09/task.bin", O_RDONLY, 0);
	if (f == NULL) {
		pr_info("filp_open error!\n");
		return -EIO;
	}
	fs = get_fs();
	set_fs(get_ds());
	f->f_op->read(f, buffer, 10000, &f->f_pos);
	set_fs(fs);
	pr_info("Loading task.bin to buffer finished\n");
	filp_close(f, NULL);
	misc_register(&my_device);
	return 0;
}

static void my_exit(void)
{
	vfree(buffer);
	vfree(temp1);
	vfree(temp2);
	free_page((unsigned long)temp3);
	free_page((unsigned long)temp4);
	misc_deregister(&my_device);
	pr_info("Removing module.\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
