/*
 * Core Source for:
 * CY8CTMA4XX
 *
 * Copyright (C) 2012 Pantech, Inc.
 * 
 * dependent to cyttsp4_pantech.c
 */

/* -------------------------------------------------------------------- */
/* for SkyTestMenu */
/* -------------------------------------------------------------------- */
#ifdef  SKY_PROCESS_CMD_KEY
static long touch_fops_ioctl(struct file *filp,unsigned int cmd, unsigned long arg)
{
    void __user *argp = (void __user *)arg;
    int return_to_user = 0;    

    mutex_lock(&cyttsp4_data->data_lock);
    switch (cmd) 
    {
    case TOUCH_IOCTL_READ_LASTKEY:
        break;
    case TOUCH_IOCTL_DO_KEY:
        pr_info("TOUCH_IOCTL_DO_KEY  = %d\n",(int)argp);			
        if ( (int)argp == 0x20a )
            input_report_key(cyttsp4_data->input, 0xe3, 1);
        else if ( (int)argp == 0x20b )
            input_report_key(cyttsp4_data->input, 0xe4, 1);
        else
            input_report_key(cyttsp4_data->input, (int)argp, 1);
        input_sync(cyttsp4_data->input);
        break;
    case TOUCH_IOCTL_RELEASE_KEY:		
        pr_info("TOUCH_IOCTL_RELEASE_KEY  = %d\n",(int)argp);
        if ( (int)argp == 0x20a )
            input_report_key(cyttsp4_data->input, 0xe3, 0);
        else if ( (int)argp == 0x20b )
            input_report_key(cyttsp4_data->input, 0xe4, 0);
        else
            input_report_key(cyttsp4_data->input, (int)argp, 0);
        input_sync(cyttsp4_data->input);
        break;		
    case TOUCH_IOCTL_DEBUG:
        pr_info("Touch Screen Read Queue ~!!\n");			
        break;
    case TOUCH_IOCTL_CLEAN:
        pr_info("Touch Screen Previous Data Clean ~!!\n");
        break;
    case TOUCH_IOCTL_RESTART:
        pr_info("Touch Screen Calibration Restart ~!!\n");	
#ifdef CY_USE_MANUAL_CALIBRATION
        cyttsp4_manual_calibration(cyttsp4_data);
#endif /* --CY_USE_MANUAL_CALIBRATION */
        break;
    case TOUCH_IOCTL_PRESS_TOUCH:
        input_report_key(cyttsp4_data->input, BTN_TOUCH, 1);
        input_report_abs(cyttsp4_data->input, ABS_MT_TOUCH_MAJOR, 255);
        input_report_abs(cyttsp4_data->input, ABS_MT_POSITION_X, (int)(arg&0x0000FFFF));
        input_report_abs(cyttsp4_data->input, ABS_MT_POSITION_Y, (int)((arg >> 16) & 0x0000FFFF));
        input_report_abs(cyttsp4_data->input, ABS_MT_WIDTH_MAJOR, 1);
        input_mt_sync(cyttsp4_data->input);
        input_sync(cyttsp4_data->input);
        break;
    case TOUCH_IOCTL_RELEASE_TOUCH:		
        input_report_key(cyttsp4_data->input, BTN_TOUCH, 0);			
        input_report_abs(cyttsp4_data->input, ABS_MT_TOUCH_MAJOR, 0);
        input_report_abs(cyttsp4_data->input, ABS_MT_POSITION_X, (int)(arg&0x0000FFFF));
        input_report_abs(cyttsp4_data->input, ABS_MT_POSITION_Y, (int)((arg >> 16) & 0x0000FFFF));
        input_report_abs(cyttsp4_data->input, ABS_MT_WIDTH_MAJOR, 1);
        input_mt_sync(cyttsp4_data->input);
        input_sync(cyttsp4_data->input); 
        break;
    case TOUCH_IOCTL_CHARGER_MODE:
#ifdef CY_USE_CHARGER_MODE
        return_to_user = cyttsp4_chargermode(cyttsp4_data,arg);
#endif
        break;
    case POWER_OFF:
        break;
    case TOUCH_IOCTL_DELETE_ACTAREA:
        break;
    case TOUCH_IOCTL_RECOVERY_ACTAREA:
        break;
    case TOUCH_IOCTL_STYLUS_MODE:
        break;
    case TOUCH_CHARGE_MODE_CTL:
        break;
    case TOUCH_IOCTL_SENSOR_X:
        return_to_user = X_SENSOR_NUM;
        if(copy_to_user(argp, &return_to_user, sizeof(int)))
            pr_err("%s: Oops..\n",__func__);
        break;
    case TOUCH_IOCTL_SENSOR_Y:
        return_to_user = Y_SENSOR_NUM;
        if(copy_to_user(argp, &return_to_user, sizeof(int)))
            pr_err("%s: Oops..\n",__func__);
        break;
    case TOUCH_IOCTL_CHECK_BASE:
        send_reference_data(arg);
        break;
    case TOUCH_IOCTL_SELF_TEST:
#ifdef CY_USE_GLOBAL_IDAC
        return_to_user = cyttsp4_global_idac_test(cyttsp4_data);
#endif /* --CY_USE_GLOBAL_IDAC */
#ifdef CY_USE_OPEN_TEST
        return_to_user = cyttsp4_open_test(cyttsp4_data);
#endif /* --CY_USE_OPEN_TEST */
#ifdef CY_USE_PANEL_TEST
        return_to_user = cyttsp4_panel_test(cyttsp4_data);
#endif /* --CY_USE_PANEL_TEST */

//++ p11309
#ifdef CONFIG_MACH_MSM8960_SIRIUSLTE
		if ( return_to_user == 0 ) return_to_user = true;
		else return_to_user = false;
		if(copy_to_user(argp, &reference_data, sizeof(int) * X_SENSOR_NUM * Y_SENSOR_NUM )) {
			pr_err("%s: copy_to_user Error..\n",__func__);
			return_to_user = false;
		}
#else
		if(copy_to_user(argp, &return_to_user, sizeof(int)))
            pr_err("%s: Oops..\n",__func__);
#endif

		break;
	case TOUCH_IOCTL_RAW_MIN:
		return_to_user = SELF_TEST_RAW_MIN;
		if(copy_to_user(argp, &return_to_user, sizeof(int)))
			pr_err("%s: Oops..TOUCH_IOCTL_RAW_MIN\n",__func__);		
		break;
	case TOUCH_IOCTL_RAW_MAX:
		return_to_user = SELF_TEST_RAW_MAX;
		if(copy_to_user(argp, &return_to_user, sizeof(int)))
			pr_err("%s: Oops..TOUCH_IOCTL_RAW_MAX\n",__func__);
		
		break;
//-- p11309
    default:
        break;
    }
    mutex_unlock(&cyttsp4_data->data_lock);

	pr_info("[+++ touch_fops]cmd=%d, arg=%ld, return=%d\n",cmd, arg, return_to_user);

    return return_to_user;
}

static ssize_t touch_fops_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
    int nBufSize=0;
    if((size_t)(*ppos) > 0) return 0;
    if(buf!=NULL) {
        nBufSize=strlen(buf);
#ifdef CONFIG_TOUCHSCREEN_DEBUG
        if(strncmp(buf, "debug", 5)==0)
        {			
			cyttsp4_data->bus_ops->tsdebug = 3;
        }
        if(strncmp(buf, "debugoff", 8)==0)
        {
			cyttsp4_data->bus_ops->tsdebug = 0;
        }
		if(strncmp(buf, "d0", 2)==0) {
            cyttsp4_data->bus_ops->tsdebug = 0;
        }
        if(strncmp(buf, "d1", 2)==0) {
            cyttsp4_data->bus_ops->tsdebug = 1;
        }
        if(strncmp(buf, "d2", 2)==0) {
            cyttsp4_data->bus_ops->tsdebug = 2;
        }
        if(strncmp(buf, "d3", 2)==0) {
            cyttsp4_data->bus_ops->tsdebug = 3;
        }
        if(strncmp(buf, "cal", 3)==0) {
#ifdef CY_USE_MANUAL_CALIBRATION
            mutex_lock(&cyttsp4_data->data_lock);
            cyttsp4_manual_calibration(cyttsp4_data);
            mutex_unlock(&cyttsp4_data->data_lock);
#endif /* --CY_USE_MANUAL_CALIBRATION */
        }
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */
    }
    *ppos +=nBufSize;
    return nBufSize;
}

static struct file_operations touch_fops = {
    .owner = THIS_MODULE,
#if ((LINUX_VERSION_CODE & 0xFFFF00) < KERNEL_VERSION(3,0,0))
    .ioctl = touch_fops_ioctl,
#else
    .unlocked_ioctl = touch_fops_ioctl,
#endif
    .write = touch_fops_write,
};

static struct miscdevice touch_fops_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "touch_fops",
    .fops = &touch_fops,
};

// call in driver init function
void touch_fops_init(void) {
	int rc;
	rc = misc_register(&touch_fops_dev);
	if (rc) {
		pr_err("::::::::: can''t register touch_fops\n");
	}
}

// call in driver remove function
void touch_fops_exit(void) {
	misc_deregister(&touch_fops_dev);
}
#endif /* --SKY_PROCESS_CMD_KEY */
