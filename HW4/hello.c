
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
	 struct list_head *node;
	 ktime_t  time;
};

struct data *item;
struct list_head * iter;

LIST_HEAD(head);



static int __init hello_init(void)
{
	if (printAmount>=5 && printAmount<=10 )
	    {
	    	printk(KERN_WARNING "printAmount between 5 and 10 !\n");
	    }
	if (printAmount>10)
	    {
	        printk(KERN_EMERG "Error, input exceed 10!\n");
	        return -EINVAL;
	    }
	while(printAmount)
	{
	    item = kmalloc(sizeof(*item), GFP_KERNEL);
	    if(item!=NULL)
	    {
	    	  item->time = ktime_get();
	    	  list_add(item->node, &head);
	    	  printk("kmalloc");
	    	
	    }
	  
	    printk(KERN_EMERG "Hello, world!\n");
	    printAmount--;
	    
	}

	return 0;
}

static void __exit hello_exit(void)
{
	list_for_each(iter,item->node) {
		item = list_entry (iter, struct data, node);
		printk(KERN_EMERG "%lld\n", item->time);

}
}

module_init(hello_init);
module_exit(hello_exit);
