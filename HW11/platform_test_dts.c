#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <asm/io.h>



#define DRV_NAME  "plat_dummy"
/*Device has 2 resources:
* 1) 4K of memory at address defined by dts - used for data transfer;
* 2) Two 32-bit registers at address (defined by dts)
*  2.1. Flag Register: @offset 0
*	bit 0: PLAT_IO_DATA_READY - set to 1 if data from device ready
*	other bits: reserved;
* 2.2. Data size Register @offset 4: - Contain data size from device (0..4095);
*/
#define MEM_SIZE	(4096)
#define REG_SIZE	(8)
#define DEVICE_POOLING_TIME_MS (500) /*500 ms*/
/*for read*/
#define PLAT_IO_FLAG_REG		(0) /*Offset of flag register*/
#define PLAT_IO_SIZE_REG		(4) /*Offset of flag register*/
#define PLAT_IO_DATA_READY	(1) /*IO data ready flag */
#define MAX_DUMMY_PLAT_THREADS 1 /*Maximum amount of threads for this */
/*for write*/
#define TIME_SIZE	(4)







//homework(write data from kernel to device) read from dev/mem -> create buffer for data in dev/mem, adress range for reg counter and  synch flag, and we need separated workqueue, which will send data from kernel(e.g. jiffies values with some period)

// and create userspace program which will read data from device(dev/mem)

//list of resourses of the device 
struct plat_dummy_device {
	void __iomem *mem;
	void __iomem *regs;
	void __iomem *time_reg;//need to map
	struct delayed_work     dwork;
	struct workqueue_struct *data_read_wq;
	u64 js_pool_time;
	u64 jiffies_to_send;//data to sent in devmem
	spinlock_t lock;
};

/*The proper way of getting at I/O memory is via a set of functions (defined via <asm/io.h>) provided for that purpose.*/
static u32 plat_dummy_mem_read8(struct plat_dummy_device *my_dev, u32 offset)
{
	return ioread8(my_dev->mem + offset);
}

static u32 plat_dummy_reg_read32(struct plat_dummy_device *my_dev, u32 offset)
{
	return ioread32(my_dev->regs + offset);
}
static void plat_dummy_reg_write32(struct plat_dummy_device *my_dev, u32 offset, u32 val)
{
	iowrite32(val, my_dev->regs + offset);
}

static void plat_dummy_work(struct work_struct *work)//callback function of workqueue
{
	struct plat_dummy_device *my_device;
	u32 i, size, status;
	u8 data;
		
	pr_info("++%s(%u)\n", __func__, jiffies_to_msecs(jiffies));

	my_device = container_of(work, struct plat_dummy_device, dwork.work);
	status = plat_dummy_reg_read32(my_device, PLAT_IO_FLAG_REG);
	//plat_dummy_reg_write32(my_device, PLAT_IO_FLAG_REG+TIME_SIZE, );
	iowrite32(jiffies, my_device->time_reg);
	pr_info("dev/mem/ write ok\n");
	

	if (status & PLAT_IO_DATA_READY) {
		size = plat_dummy_reg_read32(my_device, PLAT_IO_SIZE_REG);
		pr_info("%s: size = %d\n", __func__, size);

		if (size > MEM_SIZE)
			size = MEM_SIZE;

		for(i = 0; i < size; i++) {
			data = plat_dummy_mem_read8(my_device, i);
			pr_info("%s: mem[%d] = 0x%x ('%c')\n", __func__,  i, data, data);
		}
		rmb();
		
		status &= ~PLAT_IO_DATA_READY;
		plat_dummy_reg_write32(my_device, PLAT_IO_FLAG_REG, status);//change status of the data buffer, back to zero

	}
	//queue_delayed_work(my_device->data_read_wq, &my_device->dwork, my_device->js_pool_time);//restart
		//iowrite32(jiffies, my_device->jiffies_to_send);//write to /dev/mem/
	
	
}

	/*static inline void plat_dummy_write_current_time(struct plat_dummy_device *dev)
{
	plat_dummy_time_reg_lock(dev);
	iowrite32(jiffies, dev->time_reg);
	plat_dummy_time_reg_unlock(dev);
}*/



static const struct of_device_id plat_dummy_of_match[] = {
	{
		.compatible = "ti,plat_dummy",
	}, {
	},
 };

static int plat_dummy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct plat_dummy_device *my_device;
	struct resource *res;
	struct device_node *np = pdev->dev.of_node;

	pr_info("++%s\n", __func__);

	if (!np) {
		pr_info("No device node found!\n");
		return -ENOMEM;
	}
	pr_info("Device name: %s\n", np->name);
	
	/*
	
	kzalloc() allocates kernel memory like kmalloc(), but it also zero-initializes the allocated memory.
	The memory allocated with devm_kzalloc() is freed automatically.
	*/
	my_device = devm_kzalloc(dev, sizeof(struct plat_dummy_device), GFP_KERNEL);//create instance of device driver.
	if (!my_device)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	pr_info("res 0 = %zx..%zx\n", res->start, res->end);


	my_device->mem = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(my_device->mem))
		return PTR_ERR(my_device->mem);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);//get a resource for a  device .
	pr_info("res 1 = %zx..%zx\n", res->start, res->end);
	
	

	my_device->regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(my_device->regs))
		return PTR_ERR(my_device->regs);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 2);//get a resource for a  device .
	pr_info("res 2 = %zx..%zx\n", res->start, res->end);
	
	my_device->time_reg = devm_ioremap_resource(&pdev->dev, res);

	platform_set_drvdata(pdev, my_device);//add date related to my device to the device struct

	pr_info("Memory mapped to %p\n", my_device->regs);
	pr_info("Registers mapped to %p\n", my_device->mem);
	pr_info("time_reg mapped to %p\n", my_device->time_reg);
	

	/*Init data read WQ*/
	my_device->data_read_wq = alloc_workqueue("plat_dummy_read",
					WQ_UNBOUND, MAX_DUMMY_PLAT_THREADS);

	if (!my_device->data_read_wq)
		return -ENOMEM;

	INIT_DELAYED_WORK(&my_device->dwork, plat_dummy_work);
	my_device->js_pool_time = msecs_to_jiffies(DEVICE_POOLING_TIME_MS);
	queue_delayed_work(my_device->data_read_wq, &my_device->dwork, 0);

	return PTR_ERR_OR_ZERO(my_device->mem);
}

static int plat_dummy_remove(struct platform_device *pdev)
{
	struct plat_dummy_device *my_device = platform_get_drvdata(pdev);

	pr_info("++%s\n", __func__);

	if (my_device->data_read_wq) {
	/* Destroy work Queue */
		cancel_delayed_work_sync(&my_device->dwork);
		destroy_workqueue(my_device->data_read_wq);
	}

        return 0;
}

static struct platform_driver plat_dummy_driver = {
	.driver = {
		.name = DRV_NAME,
		.of_match_table = plat_dummy_of_match,
	},
	.probe		= plat_dummy_probe,
	.remove		= plat_dummy_remove,
};

MODULE_DEVICE_TABLE(of, plat_dummy_of_match);

module_platform_driver(plat_dummy_driver);

MODULE_AUTHOR("Vitaliy Vasylskyy <vitaliy.vasylskyy@globallogic.com>");
MODULE_DESCRIPTION("Dummy platform driver");
MODULE_LICENSE("GPL");
