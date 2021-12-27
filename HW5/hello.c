#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#define DEBUG

MODULE_AUTHOR("alex170104");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");


int bug_indicator;

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
	     BUG_ON(KERN_WARNING "printAmount between 5 and 10 !\n");
	}
	
	if (printAmount > 10)
	{
	     BUG_ON(KERN_EMERG "Error, input exceed 10!\n");
	     return -EINVAL;
	}
	
	while(printAmount)
	{
	     item = kmalloc(sizeof(*item), GFP_KERNEL);
	     if(item != NULL)
	     {
	     	   if (bug_indicator==5)
	     	   {
	     	   	BUG_ON(item);
	     	   }
	    	   bug_indicator++;
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
	pr_debug("hello");
	list_for_each_entry_safe(item,tmp, &head,node)
	{
            list_del(&(item->node));
	    pr_debug(KERN_EMERG "%lld\n", item->time);
	    kfree(item);
		
		
	}
	pr_debug("BUE");
	
}

module_init(hello_init);
module_exit(hello_exit);
