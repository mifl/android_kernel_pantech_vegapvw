/* vendor/semc/hardware/felica/msm_felica_cen.c
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
#include <linux/stddef.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <crypto/hash.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/felica.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/syscalls.h>
#include <mach/msm_smsm.h>
#include <linux/types.h>

#include "felica_master.h"


#define PRT_NAME "snfc smem"
#define WRITE_LEN 1

#define SNFC_UART_STATUS_INIT                   0x0A0A0A0A
#define SNFC_UART_STATUS_FINISH                 0xB1B1B1B1

static uint32_t *snfc_smem_info = NULL;

static int snfc_smem_open(struct inode *inode, struct file *file)
{
	pr_debug(PRT_NAME ": %s\n", __func__);

	return 0;
}

static int snfc_smem_release(struct inode *inode, struct file *file)
{
	pr_debug(PRT_NAME ": %s\n", __func__);

	return 0;
}


static ssize_t snfc_smem_read(struct file *file, char __user *buf,
					size_t count, loff_t *offset)
{
	char kbuf =0;
	
	pr_debug(PRT_NAME ": %s\n", __func__);

	if (WRITE_LEN != count || !buf) {
		pr_err(PRT_NAME ": Error. Invalid arg @CEN read.\n");
		return -EINVAL;
	}
	
	if (snfc_smem_info != NULL){
		pr_debug(PRT_NAME ": data =%x\n", *snfc_smem_info);
		if (*snfc_smem_info == SNFC_UART_STATUS_FINISH){
			kbuf =1;
		}else {
			pr_err(PRT_NAME ": not finish");
			kbuf =0;
		}
	}else {
		pr_err(PRT_NAME ": alloc error ");
	}
		
	if ( copy_to_user(buf, &kbuf, 1)) {
		pr_err(PRT_NAME ": Error. copy_to_user failure.\n");
		return -EFAULT;
	}
	/* 1 byte read */
	return 1;
}

static ssize_t snfc_smem_write(struct file *file, const char __user *buf,
					size_t count, loff_t *offset)
{
	unsigned int value;

	if (sizeof(unsigned int) != count  || !buf) {
		pr_err(PRT_NAME ": Error. Invalid arg @snfc_smem_write.\n");
		return -EINVAL;
	}

	/* Copy value from user space */
	if (copy_from_user(&value, buf, count)) {
		pr_err(PRT_NAME ": Error. copy_from_user failure.\n");
		return -EFAULT;
	}

	pr_err(PRT_NAME "snfc_smem_write.:0x%x \n", value);

	if (snfc_smem_info != NULL){
		*snfc_smem_info = value;
	}else {
		pr_err(PRT_NAME ": alloc error ");
	}

	return count;
}

/***************** FOPS ****************************/
static const struct file_operations snfc_smem_fops = {
	.owner		= THIS_MODULE,
	.read		= snfc_smem_read,
	.write		= snfc_smem_write,
	.open		= snfc_smem_open,
	.release	= snfc_smem_release,
};

static struct miscdevice snfc_smem_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "snfc_smem",
	.fops = &snfc_smem_fops,
};


int snfc_smem_probe_func(void)
{
	pr_debug(PRT_NAME ": %s\n", __func__);

	snfc_smem_info = smem_alloc2(SMEM_ID_VENDOR0, sizeof(uint32_t));

	/* Create CEN character device (/dev/felica_cen) */
	if (misc_register(&snfc_smem_device)) {
		pr_err(PRT_NAME ": Error. Cannot register CEN.\n");
		return(-ENOMEM);
	}
	return 0;
}

void snfc_smem_remove_func(void)
{

	pr_debug(PRT_NAME ": %s\n", __func__);

	misc_deregister(&snfc_smem_device);
}
