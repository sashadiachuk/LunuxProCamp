#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/delay.h>

struct task_struct * task[5];
struct mutex my_mutex; /* shared between the threads */
int global_var = 0;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("alex170104");

int kthread_func_1(void *data)
{
      while(!kthread_should_stop()) {

	     pr_info("thread 1 entered\n");
          mutex_lock(&my_mutex);
          global_var++;
          pr_info("thread 1 - %d\n", global_var);

          mutex_unlock(&my_mutex);
          msleep(5000);
      }
      
      return 0;


}
int kthread_func_2(void *data)
{
	while(!kthread_should_stop()) {

	     pr_info("thread 2 entered\n");
          mutex_lock(&my_mutex);
          global_var++;
          pr_info("thread 2 - %d\n", global_var);

          mutex_unlock(&my_mutex);
          msleep(5000);
      }
      
      return 0;

}
int kthread_func_3(void *data)
{
	while(!kthread_should_stop()) {

	     pr_info("thread 3 entered\n");
          mutex_lock(&my_mutex);
          global_var++;
          pr_info("thread 3 - %d\n", global_var);

          mutex_unlock(&my_mutex);
          msleep(5000);

      }
      return 0;

}
int kthread_func_4(void *data)
{
	while(!kthread_should_stop()) {

	     pr_info("thread 4 entered\n");
          mutex_lock(&my_mutex);
          global_var++;
          pr_info("thread 4 - %d\n", global_var);

          mutex_unlock(&my_mutex);
          msleep(5000);

      }
      msleep(5000);
      return 0;

}
int kthread_func_5(void *data)
{
	 while(!kthread_should_stop()) {

	     pr_info("thread 5 entered\n");
          mutex_lock(&my_mutex);
          global_var++;
          pr_info("thread 5 - %d\n", global_var);

          mutex_unlock(&my_mutex);
          msleep(5000);

      }
      return 0;

}

static int __init init_func(void)
{
     pr_info("entering the module\n");
  
     mutex_init(&my_mutex); /* called only ONCE */

     // start a kernel thread, kthread_run creates and starts the thread, kthread_create creates a thread but doesn't start it
     task[0] = kthread_run(kthread_func_1, NULL, "thread-1");
     if (task[0])
     {
          pr_info("thread 1 created and started  successfully\n");
     }
     else
     {
          pr_err("thread 1 created UNsuccessfully\n");
     }
     task[1] = kthread_run(kthread_func_2, NULL, "thread-2");
      if (task[1])
     {
          pr_info("thread 2 created and started  successfully\n");
     }
     else
     {
          pr_err("thread 2 created UNsuccessfully\n");
          kthread_stop(task[0]);   
          pr_err("All previous kthreads destroyed\n");
     }
     task[2] = kthread_run(kthread_func_3, NULL, "thread-3");
      if (task[2])
     {
          pr_info("thread 3 created and started successfully\n");
     }
     else
     {
          pr_err("thread 3 created UNsuccessfully\n");
          kthread_stop(task[0]);   
          kthread_stop(task[1]);   
          pr_err("All previous kthreads destroyed\n");
     }
     task[3] = kthread_run(kthread_func_4, NULL, "thread-4");
      if (task[3] )
     {
          pr_info("thread 4 created and started  successfully\n");
     }
     else
     {
          pr_err("thread 4 created UNsuccessfully\n");
          kthread_stop(task[0]);   
          kthread_stop(task[1]);   
          kthread_stop(task[2]);   
          pr_err("All previous kthreads destroyed\n");
     }
     task[4] = kthread_run(kthread_func_5, NULL, "thread-5");
      if (task[4])
     {
          pr_info("thread 5 created and started successfully\n");
     }
     else
     {
          pr_err("thread 5 created UNsuccessfully\n");
          kthread_stop(task[0]);   
          kthread_stop(task[1]);   
          kthread_stop(task[2]);   
          kthread_stop(task[3]);   
          pr_err("All previous kthreads destroyed\n");
     }
     
   return 0;

}
static void __exit exit_func(void)
{
    pr_info("Exiting the module\n");
    kthread_stop(task[0]);   
    kthread_stop(task[1]);   
    kthread_stop(task[2]);   
    kthread_stop(task[3]);   
    kthread_stop(task[4]);   
    pr_info("All kthreads destroyed\n");
}

module_init(init_func);
module_exit(exit_func);

