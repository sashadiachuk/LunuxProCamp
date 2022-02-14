// SPDX-License-Identifier: GPL-2.0
/*
 * Sample: add sysfs attributes for platform device.
 *
 * Manually add/remove attribute groups in probe/remove.
 *
 * Note: this way may lead to race if userspace applications
 * expect the attributes should exists when core adds the
 * device.
 *
 * Copyright (C) 2019 Illia Smyrnov <illia.smyrnov@globallogic.com>
 */

#include <linux/device.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/string.h>
#include <linux/sysfs.h>

struct sysfs_sample_data {
	int foo;
	int bar;
};

/*
 * The "foo" file where a static variable is read from and written to.
 */
static ssize_t foo_show(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct sysfs_sample_data *pdata = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", pdata->foo);
}

static ssize_t foo_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	struct sysfs_sample_data *pdata = dev_get_drvdata(dev);
	int ret;

	ret = kstrtoint(buf, 10, &pdata->foo);
	if (ret < 0) {
		dev_err(dev, "%s: invalid input, %d\n",  __func__, ret);
		return ret;
	}

	return count;
}
static DEVICE_ATTR_RW(foo);

/*
 * The "bar" file where a static variable is read only.
 */
static ssize_t bar_show(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct sysfs_sample_data *pdata = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", pdata->bar);
}
static DEVICE_ATTR_RO(bar);

/*
 * The "baw" file where a static variable is write only.
 */
static ssize_t baw_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	struct sysfs_sample_data *pdata = dev_get_drvdata(dev);
	int ret;

	ret = kstrtoint(buf, 10, &pdata->bar);
	if (ret < 0) {
		dev_err(dev, "%s: invalid input, %d\n",  __func__, ret);
		return ret;
	}

	return count;
}
static DEVICE_ATTR_WO(baw);

/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *sysfs_sample_attrs[] = {
	&dev_attr_foo.attr,
	&dev_attr_bar.attr,
	&dev_attr_baw.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};
ATTRIBUTE_GROUPS(sysfs_sample);

static struct sysfs_sample_data sysfs_sample_data;

static int sysfs_sample_probe(struct platform_device *pdev)
{
	int ret;

	dev_set_drvdata(&pdev->dev, &sysfs_sample_data);

	ret = sysfs_create_groups(&pdev->dev.kobj, sysfs_sample_groups);
	if (ret) {
		dev_err(&pdev->dev, "%s: failed to create sysfs group %s, %d\n",
			sysfs_sample_group.name, __func__, ret);
		return ret;
	}

	return 0;
}

static int sysfs_sample_remove(struct platform_device *pdev)
{
	sysfs_remove_groups(&pdev->dev.kobj, sysfs_sample_groups);

	return 0;
}
//sysfs-sample
static struct platform_driver sysfs_sample_driver = {
	.driver		= {
		.name	= "plat_dummy",
	},
	.probe		= sysfs_sample_probe,
	.remove		= sysfs_sample_remove,
};

module_platform_driver(sysfs_sample_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Illia Smyrnov <illia.smyrnov@globallogic.com>");
