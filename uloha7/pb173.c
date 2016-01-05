#include <linux/io.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/pci.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/interrupt.h>

#define RAISED_INTR (virt_address + 0x0024)
#define RAISE_INTR (virt_address + 0x0060)
#define ACK_INTR (virt_address + 0x0064)
#define SOURCE_ADDRESS (virt_address + 0x0080)
#define DEST_ADDRESS (virt_address + 0x0088)
#define TRANSFER_COUNT (virt_address + 0x0090)
#define CMD_REG (virt_address + 0x0098)
#define EDU_MEMORY 0x40000
#define DMA_ADDRESS ((long long unsigned int) dma_virt)

void *irq_id;
void *virt_address;

struct my_list {
	struct list_head list;
	struct pci_dev *pdev;
};

struct list_head my_head;

int compare(struct pci_dev *first, struct pci_dev *second)
{
	if(pci_domain_nr(first->bus) == pci_domain_nr(second->bus))
	{
		if(first->bus == second->bus)
		{
			if(PCI_FUNC(first->devfn) == PCI_FUNC(second->devfn))
			{
				return 1;
			}
		}
	}
	return 0;

}

int compare_with_my_list(struct pci_dev *other)
{
	struct my_list *temp;
	list_for_each_entry(temp, &my_head, list){
		if(compare(other, temp->pdev) != 0)
		{
			return 1;
		}
	}
	return 0;

}

int compare_with_system(struct pci_dev *other)
{
	struct pci_dev *temp = NULL;
	while((temp = pci_get_device(PCI_ANY_ID,PCI_ANY_ID,temp))){
		if(compare(other, temp) != 0)
		{
			return 1;
		}
	}
	return 0;
}

	
void compute_factorial(long unsigned int n)
{
	unsigned int temp;
	writel(n, virt_address + 0x0008);
	while((readl(virt_address + 0x0020) & 0x1) == 0x1)
		pr_info("computing...\n");
	temp = readl(virt_address + 0x0008);
	pr_info("factorial of %d is %u\n",(int)n,temp);
}

dma_addr_t dma;
char *dma_virt;
char *buffer;
/*
void dma_intr_tasklet(unsigned long data)
{
	pr_info("retezec +20: %s",(char*)(dma_virt + 20));
}

DECLARE_TASKLET(my_tasklet, dma_intr_tasklet, 0);

static irqreturn_t intq_handler(int irq, void *data)
{
	unsigned long int temp;

	temp = readl(RAISED_INTR);
	if(temp != 0)
	{
		pr_info("intr status: %lx", temp);
		tasklet_schedule(&my_tasklet);
		writel(temp, ACK_INTR);
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}
*/

int my_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	long unsigned int temp;
	int ret;
	int wait_count = 100;

	ret = pci_enable_device(dev);
	
	if(ret != 0)
		return -EFAULT;
	ret = pci_request_region(dev, 0, "my_bar");
	if(ret != 0)
		return -EFAULT;
	virt_address = pci_ioremap_bar(dev, 0);
	pr_info("virt_address : %p", &virt_address);
	temp = readl(virt_address);
	/* Testing EDU device. */
	pr_info("Major: %u, minor: %u\n", *((char *) &temp + 3), *((char *) &temp + 2));
	pr_info("id+revision: %lx", temp);
	writel(0x1000, virt_address + 0x0004);
	temp = readl(virt_address + 0x0004);	
	writel(temp, virt_address + 0x0004);
	pr_info("double invert: 1000 is %lx", temp);
	/* Compute factorials. */
	compute_factorial(0x3);
	compute_factorial(0x4);
	compute_factorial(0x5);
	/* Prepare DMA. */
	ret = pci_set_dma_mask(dev, DMA_BIT_MASK(32));
	if(ret != 0)
		return -EFAULT;
	ret = pci_set_consistent_dma_mask(dev, DMA_BIT_MASK(32));
	pci_set_master(dev);
	dma_virt = dma_alloc_coherent(&dev->dev, PAGE_SIZE, &dma, GFP_KERNEL);
	strcpy(dma_virt, "0123456789");
	pr_info("dma: %pad", &dma);
	pr_info("virtual: %p", dma_virt);
	pr_info("virt to phys: %llx",(unsigned long long)virt_to_phys(dma_virt));

	/* Transfer from DMA page to EDU. */
	writel(DMA_ADDRESS, SOURCE_ADDRESS);
	writel(EDU_MEMORY, DEST_ADDRESS);
	writel(10, TRANSFER_COUNT);
	writel(1, CMD_REG);
//	temp = readl(CMD_REG);
	while(readl(CMD_REG) & 1)
	{
		pr_info("transporting %d, cmd_reg: %u\n", wait_count, readl(CMD_REG));
		if(wait_count < 0)
		{
			pr_info("First write failed.");
			return -ETIME;
		}
		msleep(100);
		wait_count--;
	}
	pr_info("done, cmd_reg: %u\n",readl(CMD_REG));

	/* Transfer from EDU to DMA+10 */
	writel(EDU_MEMORY, SOURCE_ADDRESS);
	writel(DMA_ADDRESS +10 , DEST_ADDRESS);
	writel(10, TRANSFER_COUNT);
	writel(3, CMD_REG);
	wait_count = 100;
	while(readl(CMD_REG) & 1)
	{
		pr_info("transporting %d, cmd_reg: %u\n", wait_count, readl(CMD_REG));
		if(wait_count < 0)
		{
			pr_info("Second write failed.");
			return -ETIME;
		}
		msleep(100);
		wait_count--;
	}
	pr_info("done, cmd_reg: %u\n",readl(CMD_REG));

	pr_info("retezec after transfer: %s\n",(char*) dma_virt);
	pr_info("retezec+10: %s\n",(char*)(dma_virt + 10));

	/* Transfer from EDU to DMA+20 with RQINT when finished. */
//	tasklet_schedule(&my_tasklet);
//	writel(EDU_MEMORY, SOURCE_ADDRESS);
//	writel((unsigned long long )(dma_virt + 20), DEST_ADDRESS);
//	writel(0x10, TRANSFER_COUNT);
//	writel(7, CMD_REG);
//	ret = request_irq(dev->irq, intq_handler, IRQF_SHARED, "my_awesome_name", &irq_id);
	return 0;
}

void my_remove(struct pci_dev *dev)
{
	dma_free_coherent(&dev->dev, PAGE_SIZE, dma_virt, dma);
//	tasklet_kill(&my_tasklet);
	pci_iounmap(dev, virt_address);
	pci_release_region(dev, 0);
	pci_disable_device(dev);
}

struct pci_device_id my_table[] = {
{ PCI_DEVICE(0x1234, 0x11e8) },
{ 0, }
};

MODULE_DEVICE_TABLE(pci, my_table);

struct pci_driver my_pci_driver = {
	.name = "my_driver",
	.id_table = my_table,
	.probe = my_probe,
	.remove = my_remove,
};

static int my_init(void)
{
	struct pci_dev *pdev = NULL;
	struct my_list *temp;
	int ret;
	pr_info("Adding module\n");
	INIT_LIST_HEAD(&my_head);

	while((pdev = pci_get_device(PCI_ANY_ID,PCI_ANY_ID,pdev)))
	{
		temp = kmalloc(sizeof(*temp), GFP_KERNEL);
		temp->pdev = pdev;
		pci_dev_get(pdev);
		list_add_tail(&temp->list, &my_head);
	}
	ret = pci_register_driver(&my_pci_driver);
	if(ret != 0)
		return -EFAULT;
	return 0;
}

static void my_exit(void)
{
	struct list_head *temp = NULL;
	struct list_head *temp2 = NULL;
	struct pci_dev *pdev = NULL;
	struct my_list *temp_my_list;
	while((pdev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, pdev)))
	{
		if(compare_with_my_list(pdev) == 0)
			pr_info("%2.x : %2.x -added \n", pdev->vendor, pdev->device);
	}
	list_for_each_entry(temp_my_list, &my_head, list)
	{
		if(compare_with_system(temp_my_list->pdev) == 0)
			pr_info("%2.x : %2.x -removed \n", temp_my_list->pdev->vendor, temp_my_list->pdev->device);
	}
	temp = NULL;
	list_for_each_entry(temp_my_list, &my_head, list)
	{
		pci_dev_put(temp_my_list->pdev);
	}
	list_for_each_safe(temp, temp2, &my_head)
	{
		list_del(temp);
		kfree(temp);
	}
	pr_info("all devices:");
	while((pdev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, pdev)))
	{
		pr_info("%2.x : %2.x  \n", pdev->vendor, pdev->device);
	}
	pci_unregister_driver(&my_pci_driver);
	pr_info("Removing module\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
