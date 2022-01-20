#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");
static int listMembers;
static struct timer_list my_timer;
static struct timer_list irq_simulation_timer;

DEFINE_SPINLOCK(list_spinlock);

struct list_data{
    spinlock_t *list_spinlock;
	ktime_t current_time;
    int current_number;
    struct list_head node;
	
};


struct data {
	ktime_t time ;
	int number;
	struct list_data *list;// list->head(next,prev) - usually we have here _"struct list_head node"_,which is 
	//conteins pointers to the prev and next element in the list
	// but here we should make one more step to list_head node, and before accessing the list we acquire spinlock
};

static LIST_HEAD(head);

struct data *cursor;//current node pointer



static void timer_callback(struct timer_list *timer)
{
	static int times = 15;
	pr_info("timer_callback called (%lu).\n", jiffies);
	
	cursor->list->current_number++;
	cursor->list->current_time = ktime_get();

	/*if(cursor->list->node->next->number == 0)//here we have our cursor in the last element, which is should points to the first element via *next pointer
	{
		cursor->list->node->next->number = cursor->list->current_number;
		cursor->list->node->next->time = cursor->list->current_time;
	}*/
	
	if(--times)// Re-enabling timer.
	{
	    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
	}

}
static void irq_simulation_callback(struct timer_list *timer)
{
	static int times = 5;
	pr_info("irq_simulation_callback called (%lu).\n", jiffies);

	spin_lock(cursor->list->list_spinlock);
	//here we will lock list data via lock, because with list data, also works timer calback function

	spin_unlock(cursor->list->list_spinlock);


	if(--times)// Re-enabling timer.
	{
	    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2500));
	}
}

static __init int simple_wq_init(void)
{	
	pr_info("Module installed\n");
	listMembers = 5;
	
	while(listMembers)
	{
		cursor = kmalloc(sizeof(*cursor), GFP_KERNEL);
		if(cursor != NULL)
	    {
			
			
			cursor->number = 0;
			cursor->time = 0;
				
			list_add(&(cursor->list->node), &head);

			
			
		}

		listMembers--;
	}
	pr_info("Timer  installing\n");

	timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
	
	pr_info("Irq simulation timer installing\n");
	
	timer_setup(&irq_simulation_timer, irq_simulation_callback, 0);
    mod_timer(&irq_simulation_timer, jiffies + msecs_to_jiffies(2500));
	

	return 0;
}

static __exit void simple_wq_exit(void)
{
	pr_info("Module uninstalled\n");

	return;
}

module_init(simple_wq_init);
module_exit(simple_wq_exit);

