/* vendor/semc/hardware/felica/felica_master.c
 *
 * Copyright (C) 2010 Sony Ericsson Mobile Communications AB.
 *
 * Author: Hiroaki Kuriyama <Hiroaki.Kuriyama@sonyericsson.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/kdev_t.h>
#ifdef CONFIG_CXD2235AGC_NFC_FELICA
#include <linux/felica.h>
#endif
#include "felica_master.h"

#define DRV_NAME "felica driver"
#define DRV_VERSION "0.2"
#define PRT_NAME "felica master"

struct felica_device *felica_drv=NULL;
struct class *felica_class;

int set_felica_attr(struct device *child, struct device_attribute *sd, int array_size)
{
	int i;

	for (i=0 ; i < array_size ; i++)
	{
		if ((device_create_file(child, &sd[i])) < 0)
		{
			pr_err(PRT_NAME ": Error. device_create_file(child, &sd[%d])\n", i);
			goto sysfs_err;
		}
	}
	return 0;
sysfs_err:
	for (i=0 ; i < array_size ; i++)
		device_remove_file(child, &sd[i]);

	return -1;
}

int felica_sysfs_register(struct felica_sysfs_dev *pdev)
{
	int ret = 0;
	dev_t dev;

	if (!felica_class)
	{
		felica_class = class_create(THIS_MODULE, "felica");
		if(IS_ERR(felica_class))
			return PTR_ERR(felica_class);
	}

	dev = MKDEV(0, pdev->minor);
	
	pdev->dev = device_create(felica_class, NULL, dev, NULL, "%s", pdev->name);

	if (IS_ERR(pdev->dev)) {
		ret = PTR_ERR(pdev->dev);
		pr_err(PRT_NAME ": Error.  device_create failed! [%d]\n", ret);
		return ret;
	}
	
	ret = set_felica_attr(pdev->dev, pdev->attr, pdev->attr_size);
	if (ret < 0)
		goto err_sys_create;

	return ret;
err_sys_create:
	device_destroy(felica_class, MKDEV(0, pdev->minor));
	class_destroy(felica_class);
	felica_class = NULL;
	return ret;
	
}

void felica_sysfs_unregister(struct felica_sysfs_dev *pdev)
{
	int i;

	for (i=0; pdev->attr_size; i++)
		device_remove_file(pdev->dev,  &pdev->attr[i]);

	device_destroy(felica_class, MKDEV(0, pdev->minor));
	class_destroy(felica_class);
}

/**
 * @brief   Probe function of FeliCa driver
 * @details This function executes;\n
 *            # Load platform data of FeliCa driver\n
 *            # GPIO setting of MSM & PM\n
 *            # Call initialization functions of each controller
 * @param   pdev     : Pointer to data of FeliCa platform device
 * @retval  0        : Success
 * @retval  Negative : Failure
 * @note
 */
static int felica_probe(struct platform_device *pdev)
{
	int	ret;
	struct felica_platform_data	*pfdata;
	struct felica_device *felica;
	pr_info(PRT_NAME ": %s start\n", __func__);

	/* Load platform data of FeliCa driver */
	pfdata = pdev->dev.platform_data;
	if (NULL == pfdata) {
		pr_err(PRT_NAME ": Error. No platform data.\n");
		ret = -EINVAL;
		goto err_get_platform_data;
	}

	felica = kzalloc(sizeof(struct felica_device), GFP_KERNEL);
	if (!felica) {
		pr_err(PRT_NAME ": Unable to allocate memory\n");
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, felica);
	felica_drv = felica;
	felica->pdev = pdev;
	
	ret = felica_uart_probe_func(felica);
	if (ret) {
		pr_err(PRT_NAME ": Error. FELICA probe failure.\n");
		goto err_felica_uart_probe;
	}
	
	ret = felica_cen_probe_func(felica);
	if (ret) {
		pr_err(PRT_NAME ": Error. CEN probe failure.\n");
		goto err_cen_probe;
	}

	ret = felica_pon_probe_func(felica);
	if (ret) {
		pr_err(PRT_NAME ": Error. PON probe failure.\n");
		goto err_pon_probe;
	}

	ret = felica_rfs_probe_func(felica);
	if (ret) {
		pr_err(PRT_NAME ": Error. RFS probe failure.\n");
		goto err_rfs_probe;
	}

	ret = felica_int_probe_func(felica);
	if (ret) {
		pr_err(PRT_NAME ": Error. INT probe failure.\n");
		goto err_int_probe;
	}

	ret = felica_rws_probe_func();
	if (ret) {
		pr_err(PRT_NAME ": Error. RWS probe failure.\n");
		goto err_rws_probe;
	}

	ret = snfc_pwr_probe_func(felica);
	if (ret) {
		pr_err(PRT_NAME ": Error. SNFC probe failure.\n");
		goto err_snfc_uart_probe;
	}

	ret = snfc_intu_probe_func(felica);
	if (ret) {
		pr_err(PRT_NAME ": Error. RWS probe failure.\n");
		goto err_intu_probe;
	}

	ret = snfc_hsel_probe_func(felica);
	if (ret) {
		pr_err(PRT_NAME ": Error. RWS probe failure.\n");
		goto err_hsel_probe;
	}

	ret = snfc_uart_avail_probe_func(felica);
	if (ret) {
		pr_err(PRT_NAME ": Error. RWS probe failure.\n");
		goto err_avail_probe;
	}

	ret = snfc_smem_probe_func();
	if (ret){
		pr_err(PRT_NAME ": Error. RWS probe failure.\n");
		goto err_smem_probe;
	}
	pr_info(PRT_NAME ": %s end\n", __func__);
	return 0;

/* Error handling */
err_smem_probe:
	snfc_smem_remove_func();
err_avail_probe:
	snfc_uart_avail_remove_func();
err_hsel_probe:
	snfc_intu_remove_func();
err_intu_probe:
	snfc_pwr_remove_func();
err_snfc_uart_probe:	
	felica_rws_remove_func();
err_rws_probe:
	felica_int_remove_func();
err_int_probe:
	felica_rfs_remove_func();
err_rfs_probe:
	felica_pon_remove_func();
err_pon_probe:
	felica_cen_remove_func();
err_cen_probe:
	felica_uart_remove_func();
err_felica_uart_probe:
	kfree(felica);
err_get_platform_data:
	return ret;
}

/**
 * @brief   Remove function of FeliCa driver
 * @details This function executes;\n
 *            # Call termination functions of each controller
 * @param   pdev : (unused)
 * @retval  0    : Success
 * @note
 */
static int felica_remove(struct platform_device *pdev)
{
	pr_debug(PRT_NAME ": %s\n", __func__);

	snfc_smem_remove_func();
	snfc_uart_avail_remove_func();
	snfc_intu_remove_func();
	snfc_pwr_remove_func();
	felica_rws_remove_func();
	felica_int_remove_func();
	felica_rfs_remove_func();
	felica_pon_remove_func();
	felica_cen_remove_func();
	felica_uart_remove_func();
	return 0;
}

/**
 * @brief  Suspend function of FeliCa driver
 * @param  pdev : (unused)
 * @param  message
 * @retval 0
 * @note
 */
static int felica_suspend(struct platform_device *pdev,
					pm_message_t message)
{
	pr_debug(PRT_NAME ": %s\n", __func__);

	return 0;
}

/**
 * @brief  Resume function of FeliCa driver
 * @param  pdev : (unused)
 * @retval 0
 * @note
 */
static int felica_resume(struct platform_device *pdev)
{
	pr_debug(PRT_NAME ": %s\n", __func__);

	return 0;
}

/**
 * @brief Platform driver data structure of FeliCa driver
 */
static struct platform_driver felica_driver = {
	.probe		= felica_probe,
	.remove		= felica_remove,
	.suspend	= felica_suspend,
	.resume		= felica_resume,
	.driver		= {
		.name		= "felica",
		.owner		= THIS_MODULE,
	},
};

/**
 * @brief   Init function of FeliCa driver
 * @details This function executes;\n
 *            # platform driver registration of FeliCa driver
 * @param   N/A
 * @retval  0        : Success
 * @retval  Negative : Failure
 * @note
 */
static int __init felica_init(void)
{
	int ret;

	pr_info(PRT_NAME ": FeliCa driver ver %s being loaded.\n",
							DRV_VERSION);

	ret = platform_driver_register(&felica_driver);
	return ret;
}

/**
 * @brief   Exit function of FeliCa driver
 * @details This function executes;\n
 *            # platform driver unregistration of FeliCa driver
 * @param   N/A
 * @retval  N/A
 * @note
 */
static void __exit felica_exit(void)
{
	pr_info(PRT_NAME ": %s\n", __func__);

	platform_driver_unregister(&felica_driver);
}

module_init(felica_init);
module_exit(felica_exit);

MODULE_VERSION(DRV_VERSION);
MODULE_AUTHOR("SEMC");
MODULE_DESCRIPTION("FeliCa driver");
MODULE_LICENSE("GPL");
