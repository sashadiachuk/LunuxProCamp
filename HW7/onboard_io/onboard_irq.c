/* SPDX-License-Identifier: GPL-2.0
 *
 * BBB On-board IO demo.
 * Just on leds at init and of at exit
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksandr Redchuk (at GL training courses)");
MODULE_DESCRIPTION("BBB Onboard IO Demo");
MODULE_VERSION("0.1");
//MODULE_INFO(vermagic, "5.13.0 SMP mod_unload ARMv7 p2v8 ");

#define GPIO_NUMBER(port, bit) (32 * (port) + (bit))

/* On-board LESs
 *  0: D2	GPIO1_21	heartbeat
 *  1: D3	GPIO1_22	uSD access
 *  2: D4	GPIO1_23	active
 *  3: D5	GPIO1_24	MMC access
 *
 * uSD and MMC access LEDs are not used in nfs boot mode, but they are already requested
 * So that, we don't use gpio_request()/gpio_free() here.
 */

#define LED_SD  GPIO_NUMBER(1, 22)
#define LED_MMC GPIO_NUMBER(1, 24)

/* On-board button.
 *
 * HDMI interface must be disabled.
 */
#define BUTTON  GPIO_NUMBER(2, 8)

static int led_gpio = -1;
static int button_gpio = -1;


bool simulate_busy = 0;
module_param(simulate_busy, bool, S_IRUSR|S_IWUSR);  
     
// This directory entry will point to `/sys/kernel/debug/myModule	
static struct dentry *dir = 0;

struct dentry *dFile;

unsigned int GPIO_irqNumber;

// File `/sys/kernel/debug/example2/counter` points to this variable.
static int counter = 0;         


static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
/*need to add
  - toggle the led
  - open file in debug_fs  incr. press_counter
  
  
   led_gpio = (0x01 ^ led_toggle);  
  gpio_set_value(led_gpio, led_gpio^ 1);*/
 
  pr_info("Interrupt(Threaded Handler) : button was pressed\n");
  counter++;
  
	
 /*IRQ_HANDLED to return - if we aren`t go to use threaded irq*/
  return IRQ_WAKE_THREAD;
}
static irqreturn_t gpio_interrupt_thread_fn(int irq, void *dev_id) 
{

 /*need to add
  - open file in debug_fs and read press_counter*/
  pr_info("Interrupt(Threaded Handler) : button was pressed: %d times ", counter);
  
  if(simulate_busy)
  {
  	msleep(2000);
  	pr_info("simulation of work done!\n");
  }
  
  return IRQ_HANDLED;
}

static int led_gpio_init(int gpio)
{
	int rc;

	rc = gpio_direction_output(gpio, 0);
	if (rc)
		return rc;

	led_gpio = gpio;
	return 0;
}

static int button_gpio_init(int gpio)
{
	int rc;
	rc = gpio_request(gpio, "Onboard user button");
	if (rc)
		goto err_register;

	rc = gpio_direction_input(gpio);
	if (rc)
		goto err_input;

	button_gpio = gpio;
	pr_info("Init GPIO%d OK\n", button_gpio);
	return 0;

err_input:
	gpio_free(gpio);
err_register:
	return rc;
}

static void button_gpio_deinit(void)
{
	if (button_gpio >= 0) {
		gpio_free(button_gpio);
		pr_info("Deinit GPIO%d\n", button_gpio);
	}
}

/* Module entry/exit points */
static int __init onboard_io_init(void)
{
	int rc;
	int gpio;
	int button_state;
	//This used for storing the IRQ number for the GPIO
	
	
	printk("inited\n");
	//debugfs
	dir = debugfs_create_dir("myModule", NULL);//create dir myModule in root dir of debugfs
	if (dir == NULL) {
		printk(KERN_ERR "debugfs_create_dir\n");
		return -ENOMEM;
	}
	debugfs_create_u32("counter", S_IRUGO | S_IWUGO, dir, &counter);
    
    	
    	//button
	
	rc = button_gpio_init(BUTTON);
	if (rc) {
		pr_err("Can't set GPIO%d for button\n", BUTTON);
		goto err_button;
	}

	
	
	//irq
	GPIO_irqNumber= gpio_to_irq(button_gpio);
	pr_info("irq number to register %d",GPIO_irqNumber);

	rc = request_threaded_irq( GPIO_irqNumber,      //IRQ number
                            gpio_irq_handler,   			  //IRQ handler (Top half)
                            gpio_interrupt_thread_fn,   //IRQ Thread handler (Bottom half)
                            IRQF_TRIGGER_FALLING,        //Handler will be called in raising edge
                            "button_dev",               //used to identify the device name using this IRQ
                            NULL);                      //device id for shared IRQ
   	
       if (rc) {
		pr_err("my_device: cannot register IRQ ");
		goto err_button;
	}
      
    //led
       /* button_state = gpio_get_value(button_gpio);
	
	gpio = button_state ? LED_MMC : LED_SD;
	rc = led_gpio_init(gpio);
	if (rc) {
		pr_err("Can't set GPIO%d for output\n", gpio);
		goto err_led;
	}

	gpio_set_value(led_gpio, 1);*/
	pr_info("LED at GPIO%d ON\n", led_gpio);
	//test of debugfs 
	pr_info("LED at GPIO%d ON counter\n", counter);
	counter++;
	pr_info("LED at GPIO%d ON counter\n", counter);
	counter+=20;
	return 0;

err_led:
	button_gpio_deinit();
err_button:
	return rc;
}

static void __exit onboard_io_exit(void)
{
	if (led_gpio >= 0) {
		gpio_set_value(led_gpio, 0);
		pr_info("LED at GPIO%d OFF\n", led_gpio);
	}
	button_gpio_deinit();
	free_irq(GPIO_irqNumber,NULL);
	debugfs_remove_recursive(dir);
	printk("DEinited\n");
}

module_init(onboard_io_init);
module_exit(onboard_io_exit);

