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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksandr Redchuk (at GL training courses)");
MODULE_DESCRIPTION("BBB Onboard IO Demo");
MODULE_VERSION("0.1");

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

static int led_gpio_init(int gpio)
{
	int rc;
	gpio_free(gpio);
	rc = gpio_request(gpio, "led LED_SD or LED_MMC");
	if (rc)
		pr_info("request failed");
		return rc;
		
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
	
	rc = gpio_export(gpio);
	
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

	rc = button_gpio_init(BUTTON);
	
	if (rc) {
		pr_err("Can't set GPIO%d for button\n", BUTTON);
		goto err_button;
	}
	pr_info("test");
	button_state = gpio_get_value(button_gpio);

	pr_info("before set leds");
	gpio = button_state ? LED_MMC : LED_SD;
	
	if (rc) {
		pr_err("Can't set GPIO%d for output\n", gpio);
		goto err_button;
	}

        pr_info("before init led");
	rc = led_gpio_init(gpio);
	if (rc) {
		pr_err("Can't set GPIO%d for output\n", gpio);
		goto err_led;
	}
	pr_info("before set led");
	gpio_set_value(led_gpio, 1);
	pr_info("LED at GPIO%d ON\n", led_gpio);

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
}

module_init(onboard_io_init);
module_exit(onboard_io_exit);

