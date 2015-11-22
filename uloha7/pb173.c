#include <linux/io.h>
#include <linux/module.h>
#include <linux/delay.h>

#include <linux/pci.h>
#include <linux/list.h>


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

int my_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	pci_enable_device(dev);
	pci_request_region(dev, 0, "my_bar");
	pr_info("%p \n", pci_ioremap_bar(dev, 0));
	
	return 0;
}

void my_remove(struct pci_dev *dev)
{
	pci_release_region(dev, 0);
	pci_disable_device(dev);
}

struct pci_device_id my_table[] = {
{ PCI_DEVICE(0x1234, 0x11e8) },//TODO change to real values
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
	pr_info("Adding module\n");
	INIT_LIST_HEAD(&my_head);

	while((pdev = pci_get_device(PCI_ANY_ID,PCI_ANY_ID,pdev)))
	{
		temp = kmalloc(sizeof(*temp), GFP_KERNEL);
		temp->pdev = pdev;
		pci_dev_get(pdev);
		list_add_tail(&temp->list, &my_head);
	}
	pci_register_driver(&my_pci_driver);
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
			pr_info("%u : %u -added \n", pdev->vendor, pdev->device);
	}
	list_for_each_entry(temp_my_list, &my_head, list)
	{
		if(compare_with_system(temp_my_list->pdev) == 0)
			pr_info("%u : %u -removed \n", temp_my_list->pdev->vendor, temp_my_list->pdev->device);
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
		pr_info("%u : %u  \n", pdev->vendor, pdev->device);
	}
	pci_unregister_driver(&my_pci_driver);
	pr_info("Removing module\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
