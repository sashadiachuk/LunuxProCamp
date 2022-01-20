#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");

#define MS_TO_NS(x)     ((x) * NSEC_PER_MSEC)

static struct hrtimer hr_timer;
struct work_struct my_work;//declaration of work - hi tasklet
struct delayed_work my_work2;//declaration of delayed work - hi tasklet

struct work_struct my_work3;//declaration of work - reg tasklet
struct delayed_work my_work4;//declaration of delayed work - reg tasklet

void my_work_handler(struct work_struct *work);

static int restart = 5;
static unsigned long delay_in_ms = 200L;



void tasklet_fn(struct tasklet_struct *t )
{
        printk(KERN_INFO "Executing Regular Prioroty Tasklet Function : arg = %p\n", t);
        pr_info("scheduled work");
	schedule_work(&my_work);
	pr_info("wait for delayed work");
	schedule_delayed_work(&my_work2, msecs_to_jiffies(10000));
        
        
}
void tasklet_hi_fn(struct tasklet_struct *arg)
{
        printk(KERN_INFO "Executing hi priority Tasklet Function : arg = %p\n", arg);
        /*Scheduling work and delayed work*/
        pr_info("scheduled work, hi priority tasklet");
	schedule_work(&my_work);
	pr_info("wait for delayed work, hi priority tasklet");
	schedule_delayed_work(&my_work2, msecs_to_jiffies(10000));
}
void my_work_handler(struct work_struct * work)
{
	printk(KERN_INFO "Executing work Function : arg = %p\n", work);
	
}

	DECLARE_TASKLET(regularTasklet, &tasklet_fn);// declares and initiates tasklet
	DECLARE_TASKLET(hiPriorTasklet, &tasklet_hi_fn);
	
	DECLARE_DELAYED_WORK(my_work2, my_work_handler);//declares and initiates work - hi t
	DECLARE_WORK(my_work, my_work_handler);//declares and initiates work- hi t
	
	DECLARE_DELAYED_WORK(my_work4, my_work_handler);//declares and initiates work - reg t
	DECLARE_WORK(my_work3, my_work_handler);//declares and initiates work - reg t

enum hrtimer_restart my_hrtimer_callback( struct hrtimer *timer)
{
	pr_info("my_hrtimer_callback called (%llu).\n",
			ktime_to_ms(timer->base->get_time()));
	
	if (restart--) {
		hrtimer_forward_now(timer, ns_to_ktime(MS_TO_NS(delay_in_ms)));
		return HRTIMER_RESTART;
	}
	
	/*Scheduling Task to Tasklet*/
	tasklet_schedule(&regularTasklet);
	tasklet_hi_schedule(&hiPriorTasklet);
	
	return HRTIMER_NORESTART;
}

static __init int hrt_init(void)
{
	ktime_t ktime;

	pr_info("HR Timer module installing\n");

	ktime = ktime_set(0, MS_TO_NS(delay_in_ms));
	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hr_timer.function = &my_hrtimer_callback;

	pr_info("Starting timer to fire in %llu ms (%lu)\n",
			ktime_to_ms(hr_timer.base->get_time()) + delay_in_ms, jiffies);

	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);
	
	
	

	return 0;
}

static __exit void hrt_exit(void)
{
	int ret;

	ret = hrtimer_cancel(&hr_timer);
	if (ret)
		pr_info("The timer was still in use...\n");

	pr_info("HR Timer module uninstalling\n");
	flush_scheduled_work();
	tasklet_kill(&regularTasklet);
	tasklet_kill(&hiPriorTasklet);
	return;
}

module_init(hrt_init);
module_exit(hrt_exit);

