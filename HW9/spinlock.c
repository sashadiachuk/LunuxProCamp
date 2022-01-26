#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/timer.h>
#include <linux/spinlock_types.h>

MODULE_LICENSE("GPL");
static int listMembers;
static struct timer_list my_timer;
static struct timer_list irq_simulation_timer;

DEFINE_SPINLOCK(list_spinlock);

/*
struct list_data{
    struct list_head node;
    spinlock_t *list_spinlock;
    int current_number;
};

struct data {
	ktime_t time ;
	int number;
	struct list_data *list;// list->head(next,prev) - usually we have here _"struct list_head node"_,which is 
	//conteins pointers to the prev and next element in the list
	// but here we should make one more step to list_head node, and before accessing the list we acquire spinlock
};
*/

//type for head of the list
struct head_type{
    struct list_head head_node;
    spinlock_t list_spinlock;
    int current_number;
	int current_time;
};

//type for list date 
struct list_data_type{
	ktime_t time ;
	int number;
	struct list_head node;
};

//head - Екземпляр структури head_type створений статично
struct head_type head = {
	.head_node = LIST_HEAD_INIT(head.head_node),
	.list_spinlock = __SPIN_LOCK_UNLOCKED(head.list_spinlock),
	.current_number = 0,
	.current_time = 0,
};


/*Declare and init the head node of the linked list*/
//static LIST_HEAD(head_node);

struct list_data_type *cursor;//current node pointer
struct list_data_type *tmp;//temporary storage which is used by macro - list_for_each_entry_safe


static void timer_callback(struct timer_list *timer)
{
	static int times = 15;
	pr_info("timer_callback called (%lu).\n", jiffies);
	
	
	head.current_number++;
	head.current_time = ktime_get();

	/**
 	* list_last_entry - get the last element from a list
 	* @ptr:	the list head to take the element from.
 	* @type:	the type of the struct this is embedded in.
 	* @member:	the name of the list_head within the struct.
 	*
 	* Note, that list is expected to be not empty.
 	*/
 	spin_lock(&(head.list_spinlock));
	cursor = list_first_entry(&head.head_node, struct list_data_type, node);
	if (cursor->number == 0)
	{
		pr_info("first element in the list iter number - %d\n", cursor->number);
		cursor->number = head.current_number;
		cursor->time = head.current_time;
		list_rotate_left(&head.head_node);
		pr_info("first element in the list iter number - %d\n", cursor->number);
	}
	else
	{
		pr_info("overflow of the list with number - %d\n", head.current_number);
	}
	spin_unlock(&(head.list_spinlock));

	if(--times)// Re-enabling timer.
	{
	    mod_timer(&my_timer, jiffies + msecs_to_jiffies(500));
	}

}
static void irq_simulation_callback(struct timer_list *timer)
{
	static int times = 6;
	ktime_t time = 0;
	int num = 0;

	pr_info("IRQ_simulation_callback called (%lu).\n", jiffies);

	spin_lock(&(head.list_spinlock));
	//here we will lock list data via lock, because with list data, also works timer calback function
	list_for_each_entry(cursor, &head.head_node, node)
	{
		if (cursor->number != 0)
		{	
			time = cursor->time;
			num = cursor->number;
			cursor->number = 0;
			cursor->time = 0;
			pr_info("here");
			break;
		}
		

	}
	spin_unlock(&(head.list_spinlock));
	if(time != 0)
	{
		pr_info("time - %llu and number - %d of the first nonzero element\n", time,num);
		time = 0;
		num = 0;
	}
	else
	{
		pr_info("list contains zero element");
	}


	


	if(--times)// Re-enabling timer.
	{
	    mod_timer(&my_timer, jiffies + msecs_to_jiffies(600));
	}
}

static __init int simple_wq_init(void)
{	
	pr_info("Module installed\n");
	listMembers = 5;
	while(listMembers)
	{
		cursor = kmalloc(sizeof(*cursor), GFP_KERNEL);
		pr_info("mmaped\n");
		if(cursor != NULL)
	       {
			pr_info("not NULL\n");
			cursor->number = 0;
			cursor->time = 0;
			list_add(&cursor->node, &head.head_node);
			
			
			
			
		}

		listMembers--;
	}
	pr_info("Timer  installing\n");

	timer_setup(&my_timer, timer_callback, 0);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
	
	pr_info("Irq simulation timer installing\n");
	
	timer_setup(&irq_simulation_timer, irq_simulation_callback, 0);
	mod_timer(&irq_simulation_timer, jiffies + msecs_to_jiffies(1500));

	return 0;
}

static __exit void simple_wq_exit(void)
{
	pr_info("Module uninstalled\n");
    list_for_each_entry_safe(cursor,tmp, &head.head_node,node)
	{
		  pr_info("number %d\n", cursor->number);
          list_del(&(cursor->node));
		  
	      kfree(cursor);
		
		
	}
	pr_info("Memory freed\n");

	return;
}

module_init(simple_wq_init);
module_exit(simple_wq_exit);


