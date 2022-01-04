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
#include <linux/ktime.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("alex170104");
MODULE_DESCRIPTION("BBB Onboard IO Demo with system timer");
MODULE_VERSION("0.1");
MODULE_INFO(vermagic, "5.13.0 SMP mod_unload ARMv7 p2v8 ");




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
int rc;
int gpio;
int button_state;
static struct timer_list my_timer;
static unsigned long start;
static unsigned long delay_in_jiffies = (HZ * 200L) / MSEC_PER_SEC;

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
static void timer_callback(struct timer_list *timer)
{
	static int times = 15;
	pr_info("timer_callback called (%lu).\n", jiffies);
	
	

	button_state = gpio_get_value(button_gpio);

	if (button_state){
		gpio_set_value(led_gpio, 1);
		pr_info("LED at GPIO%d ON\n", led_gpio);
	}
	else
	{
		gpio_set_value(led_gpio, 0);
		pr_info("LED at GPIO%d OFF\n", led_gpio);
		
	
	}

	if(--times)
	{
	    mod_timer(timer, jiffies + msecs_to_jiffies(200));
	}

	


}
/* Module entry/exit points */
static int __init onboard_io_init(void)
{
	unsigned long now;
	
	rc = button_gpio_init(BUTTON);
	if (rc) {
		pr_err("Can't set GPIO%d for button\n", BUTTON);
		goto err_button;
	}
	
	pr_info("Button done!\n");
	gpio = LED_MMC;//gpio = led_gpio - segmentation faul after this line
	rc = led_gpio_init(gpio);
	if (rc) {
		pr_err("Can't set GPIO%d for output\n", gpio);
		goto err_led;
	}
	pr_info("led done!\n");
	
	//timer setup below
	

	pr_info("Timer module installing\n");

	timer_setup(&my_timer, timer_callback, 0);

	now = jiffies;
	start = now + delay_in_jiffies;
	pr_info("Starting timer to fire in %lu ms (%lu)\n", start, now);

	mod_timer(&my_timer, start);

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

	if (del_timer(&my_timer))
		pr_info("Рendng timer deleted\n");

	pr_info("Timer module uninstalling\n");
	button_gpio_deinit();
}

module_init(onboard_io_init);
module_exit(onboard_io_exit);

