#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/slab.h>

MODULE_AUTHOR("alex170104");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");



int printAmount = 1;
module_param(printAmount, int, S_IRUSR|S_IWUSR);                        
       
struct data
{
	 struct list_head node;
	 ktime_t  time;
};

struct data *item;//current node pointer
struct data *tmp;//temporary storage which is used by macro - list_for_each_entry_safe



LIST_HEAD(head);



static int __init hello_init(void)
{
	if (printAmount >= 5 && printAmount <= 10 )
	{
	     printk(KERN_WARNING "printAmount between 5 and 10 !\n");
	}
	
	if (printAmount > 10)
	{
	     printk(KERN_EMERG "Error, input exceed 10!\n");
	     return -EINVAL;
	}
	
	while(printAmount)
	{
	     item = kmalloc(sizeof(*item), GFP_KERNEL);
	     if(item != NULL)
	     {
	    	   item->time = ktime_get();
	    	   list_add(&(item->node), &head);
	    	 	    	
	     }
	  
	     printk(KERN_EMERG "Hello, world!\n");
	     printAmount--;
	    
	}

	 return 0;
}

static void __exit hello_exit(void)
{
	list_for_each_entry_safe(item,tmp, &head,node)
	{
            list_del(&(item->node));
	     printk(KERN_EMERG "%lld\n", item->time);
	     kfree(item);
		
		
	}
	
}

module_init(hello_init);
module_exit(hello_exit);
