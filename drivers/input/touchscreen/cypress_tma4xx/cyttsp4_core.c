/*
 * Core Source for:
 * Cypress TrueTouch(TM) Standard Product (TTSP) touchscreen drivers.
 * For use with Cypress Gen4 and Solo parts.
 * Supported parts include:
 * CY8CTMA884/616
 * CY8CTMA4XX
 *
 * Copyright (C) 2009-2012 Cypress Semiconductor, Inc.
 * Copyright (C) 2011 Motorola Mobility, Inc.
 * Copyright (C) 2012 Pantech, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2, and only version 2, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Contact Cypress Semiconductor at www.cypress.com <kev@cypress.com>
 *
 */

#if defined(CONFIG_MACH_MSM8960_SIRIUSLTE)
#include "siriuslte/cyttsp4_core.h"
#else 
#include "ef48/cyttsp4_core.h"
#endif

#include "cyttsp4_types.h"
#include "cyttsp4_core_func.h"
#include "cyttsp4_platform_data.h"

#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/input/mt.h> /* for protocol type b */

#ifdef CY_CHECK_ADC
#include <linux/msm_adc.h>
#include <linux/completion.h> 
#endif /* --CY_CHECK_ADC */

#include <linux/i2c.h>

#include "cyttsp4_sysfs.h"
#include "cyttsp4_pantech.h"

#ifdef CY_USE_PM8921
//++ p11309
#if defined(CONFIG_MACH_MSM8960_SIRIUSLTE) 
#include "siriuslte/cyttsp4_pm8921.h"
#else
#include "ef48/cyttsp4_pm8921.h"
#endif
//-- p11309
#endif /* --CY_USE_PM8921 */

//++ p11309
#ifdef CYTTSP4_USE_TOUCH_ID
#include <linux/mfd/pm8xxx/pm8xxx-adc.h>
static int MPP_TOUCH_ID_IS_TP20=0;
#endif

#ifdef CYTTSP4_PREVENT_REDOWNLOAD
static int download_count= 0;
#endif
//-- p11309

/* -------------------------------------------------------------------- */
/* global variables... */
/* -------------------------------------------------------------------- */
#define Init_Resume_T 100
int  ResumeCnt_T;
bool Pow_ON_T;

#ifdef CY_USE_PROTOCOL_TYPE_B

#if 0
struct cyttsp4_touch finger_info[CY_NUM_TRK_ID];
#else
struct cyttsp4_touch finger_info[CY_MAX_FINGERNUM];
#endif
static int _pantech_reset_fingerinfo(void);
#if 0
static void _pantech_release_input_dev(struct cyttsp4 *ts);
#endif
//-- p11309

#endif /* --CY_USE_PROTOCOL_TYPE_B */

#if	defined(CY_AUTO_LOAD_DDATA) || \
    defined(CY_AUTO_LOAD_MDATA) || \
    defined(CY_USE_TMA400) 
static const u8 cyttsp4_security_key[] = {
    0xA5, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD, 0x5A
};
#endif /* --CY_AUTO_LOAD_DDATA || CY_AUTO_LOAD_MDATA || CY_USE_TMA400 */

//++ p11309
#ifdef CYTTSP4_USE_TOUCH_ID
// BOARD_VER < TP20 : ADC 1.8V = 1800000
// BOARD_VER >= TP20 : ADC 0.9V = 900000                                  
#define TP10_MPP_VOLT_BASE		1800000		// SHORT VDD: 1.8V
#define TP10_MPP_VOLT_MARGIN	 300000	
#define TP20_MPP_VOLT_BASE		 900000		// 1M OHM VDD:1.8V
#define TP20_MPP_VOLT_MARGIN	 300000

static int read_touch_id(struct cyttsp4 *ts)
{
	struct pm8xxx_adc_chan_result result;
	int rc=0;	

#if (BOARD_VER < TP20)
	MPP_TOUCH_ID_IS_TP20 = 0;
	ts->platform_data = &cyttsp4_i2c_touch_platform_data;
#else
	MPP_TOUCH_ID_IS_TP20 = 1;
	ts->platform_data = &cyttsp4_i2c_touch_platform_data_tp20;
#endif

	rc = pm8xxx_adc_mpp_config_read(CYTTSP4_TOUCH_MPP_ID, ADC_MPP_1_AMUX6, &result);
	if(rc == -EINVAL){
		printk("[Touch_id] read_touch_id is failed~!! \n");
		return -EINVAL;
	}

	if ( (result.measurement >= TP20_MPP_VOLT_BASE-TP20_MPP_VOLT_MARGIN) 
		 && (result.measurement <= TP20_MPP_VOLT_BASE+TP20_MPP_VOLT_MARGIN) ) {	MPP_TOUCH_ID_IS_TP20 = 1; } 		 
	else if ( (result.measurement >= TP10_MPP_VOLT_BASE-TP10_MPP_VOLT_MARGIN) 
		&& (result.measurement <= TP10_MPP_VOLT_BASE+TP10_MPP_VOLT_MARGIN) ) {	MPP_TOUCH_ID_IS_TP20 = 0; }
	else {
		printk("[+++ Touch id] ADC Read Error: measurement=%lld, physical:%lld\n", result.measurement, result.physical);
	}	

	printk("[+++ Touch id: flag=%s] chan=%u, adc_code=%d, measurement=%lld, physical:%lld\n", 
		MPP_TOUCH_ID_IS_TP20 == 1 ? "TP20": "TP10", result.chan, result.adc_code, result.measurement, result.physical);		

	if ( MPP_TOUCH_ID_IS_TP20 == 1 ) {		
		ts->platform_data = &cyttsp4_i2c_touch_platform_data_tp20;
	}
	else {
		ts->platform_data = &cyttsp4_i2c_touch_platform_data;
	}

	return 0;
}
#endif
//-- p11309

static void _cyttsp4_pr_state(struct cyttsp4 *ts)
{
#ifdef CONFIG_TOUCHSCREEN_DEBUG
    if (ts->bus_ops->tsdebug >= CY_DBG_LVL_2) {
        dev_info(ts->dev, "%s: %s\n", __func__,
                ts->driver_state< CY_INVALID_STATE ? cyttsp4_driver_state_string[ts->driver_state] :"INVALID");
    }
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */
}

static void _cyttsp4_pr_buf(struct cyttsp4 *ts, u8 *dptr, int size, const char *data_name)
{
    return;
}

#ifdef CY_USE_PROTOCOL_TYPE_B
/* clear finger information */
static void _cyttsp4_clear_finger_info(struct cyttsp4 *ts)
{
    int i;   

//++ p11309
#if 0
    for (i=0; i<CY_NUM_TRK_ID;i++) {
		if( finger_info[i].abs[CY_TCH_T] != -1)  {
			dev_vdbg(ts->dev,"%s: Finger Up forced event\n",__func__);
			finger_info[i].abs[CY_TCH_T] = -1;
			input_mt_slot(ts->input, i);				
			input_report_abs(ts->input, ABS_MT_TRACKING_ID, finger_info[i].abs[CY_TCH_T]);
		}
	}
#else
	_pantech_reset_fingerinfo();
	for (i=0; i<CY_MAX_FINGERNUM;i++) {        
		dev_vdbg(ts->dev,"%s: Finger Up forced event\n",__func__);            
        input_mt_slot(ts->input, i);				
        input_report_abs(ts->input, ABS_MT_TRACKING_ID, finger_info[i].abs[CY_TCH_T]);        
    }
#endif
//-- p11309

    input_report_key(ts->input, BTN_TOUCH, 0 );
    input_sync(ts->input);
}
#endif /* --CY_USE_PROTOCOL_TYPE_B */

static int _cyttsp4_read_block_data(struct cyttsp4 *ts, u16 command, size_t length, void *buf, int i2c_addr, bool use_subaddr)
{
    int retval = 0;
    int tries = 0;

    if ((buf == NULL) || (length == 0)) {
        dev_err(ts->dev, "%s: pointer or length error"	" buf=%p length=%d\n", __func__, buf, length);
        retval = -EINVAL;
    } else {
        for (tries = 0, retval = -1; tries < CY_NUM_RETRY && (retval < 0);	tries++) {
            retval = ts->bus_ops->read(ts->bus_ops, command, length, buf, i2c_addr, use_subaddr);
            if (retval < 0) {
                msleep(CY_DELAY_DFLT);
                /*
                 * TODO: remove the extra sleep delay when
                 * the loader exit sequence is streamlined
                 */
                msleep(150);
            }
        }

        if (retval < 0) {
            dev_err(ts->dev, "%s: bus read block data fail (ret=%d)\n",	__func__, retval);
        }
    }

    return retval;
}

static int _cyttsp4_write_block_data(struct cyttsp4 *ts, u16 command,
                                     size_t length, const void *buf, int i2c_addr, bool use_subaddr)
{
    int retval = 0;
    int tries = 0;

    if ((buf == NULL) || (length == 0)) {
        dev_err(ts->dev, "%s: pointer or length error"	" buf=%p length=%d\n", __func__, buf, length);
        retval = -EINVAL;
    } else {
        for (tries = 0, retval = -1; tries < CY_NUM_RETRY && (retval < 0);	tries++) {
            retval = ts->bus_ops->write(ts->bus_ops, command, length, buf, i2c_addr, use_subaddr);
            if (retval < 0)
                msleep(CY_DELAY_DFLT);
        }

        if (retval < 0) {
            dev_err(ts->dev, "%s: bus write block data fail (ret=%d)\n",	__func__, retval);
        }
    }

    return retval;
}

#ifdef CY_USE_TMA400
static int _cyttsp4_wait_ready_int_no_init(struct cyttsp4 *ts, unsigned long timeout_ms)
{
    unsigned long uretval;
    int retval = 0;

    mutex_unlock(&ts->data_lock);
    uretval = wait_for_completion_interruptible_timeout(
                                                        &ts->ready_int_running, msecs_to_jiffies(timeout_ms));
    mutex_lock(&ts->data_lock);
    if (uretval == 0) {
        dev_err(ts->dev, "%s: timeout waiting for interrupt\n",__func__);
        retval = -ETIMEDOUT;
    }

    return retval;
}
#endif /* --CY_USE_TMA400 */

static int _cyttsp4_wait_int_no_init(struct cyttsp4 *ts,
                                     unsigned long timeout_ms)
{
    unsigned long uretval;
    int retval = 0;

    mutex_unlock(&ts->data_lock);
    uretval = wait_for_completion_interruptible_timeout(
                                                        &ts->int_running, msecs_to_jiffies(timeout_ms));
    mutex_lock(&ts->data_lock);
    if (uretval == 0) {
        dev_err(ts->dev, "%s: timeout waiting for interrupt\n",__func__);
        retval = -ETIMEDOUT;
    }

    return retval;
}

static int _cyttsp4_wait_int(struct cyttsp4 *ts, unsigned long timeout_ms)
{
    int retval = 0;

    INIT_COMPLETION(ts->int_running);
    retval = _cyttsp4_wait_int_no_init(ts, timeout_ms);
    if (retval < 0) {
        dev_err(ts->dev, "%s: timeout waiting for interrupt\n",__func__);
    }

    return retval;
}

static int _cyttsp4_wait_si_int(struct cyttsp4 *ts, unsigned long timeout_ms)
{
    unsigned long uretval;
    int retval = 0;

    mutex_unlock(&ts->data_lock);
    uretval = wait_for_completion_interruptible_timeout(
                                                        &ts->si_int_running, msecs_to_jiffies(timeout_ms));
    mutex_lock(&ts->data_lock);
    if (uretval == 0) {
        dev_err(ts->dev, "%s: timeout waiting for bootloader interrupt\n",	__func__);
        retval = -ETIMEDOUT;
    }

    return retval;
}

static void _cyttsp4_queue_startup(struct cyttsp4 *ts, bool was_suspended)
{
    ts->was_suspended = was_suspended;
    queue_work(ts->cyttsp4_wq, &ts->cyttsp4_resume_startup_work);
}

#if defined(CY_AUTO_LOAD_TOUCH_PARAMS) || \
	defined(CY_AUTO_LOAD_DDATA) || defined(CY_AUTO_LOAD_MDATA) || \
	defined(CY_USE_DEV_DEBUG_TOOLS) || defined(CY_USE_TMA884) || defined(CY_USE_TMA400)
static u16 _cyttsp4_calc_partial_crc(struct cyttsp4 *ts, u8 *pdata, size_t ndata, u16 crc)
{
    int i = 0;
    int j = 0;

    for (i = 0; i < ndata; i++) {
        crc ^= ((u16)pdata[i] << 8);

        for (j = 8; j > 0; --j) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc = crc << 1;
        }
    }
    return crc;
}

static void _cyttsp4_calc_crc(struct cyttsp4 *ts, u8 *pdata, size_t ndata, u8 *crc_h, u8 *crc_l)
{
    u16 crc = 0;

    if (pdata == NULL)
        dev_err(ts->dev, "%s: Null data ptr\n", __func__);
    else if (ndata == 0)
        dev_err(ts->dev, "%s: Num data is 0\n", __func__);
    else {
        /* Calculate CRC */
        crc = 0xFFFF;
        crc = _cyttsp4_calc_partial_crc(ts, pdata, ndata, crc);
        *crc_h = crc / 256;
        *crc_l = crc % 256;
    }
}
#endif /* --CY_AUTO_LOAD_TOUCH_PARAMS --CY_AUTO_LOAD_DDATA
	--CY_AUTO_LOAD_MDATA --CY_USE_DEV_DEBUG_TOOLS --CY_USE_TMA884 */

static bool _cyttsp4_chk_cmd_rdy(struct cyttsp4 *ts, u8 cmd)
{
    bool cond = !!(cmd & CY_CMD_RDY_BIT);
    dev_vdbg(ts->dev, "%s: cmd=%02X cond=%d\n", __func__, cmd, (int)cond);
    return cond;
}

static bool _cyttsp4_chk_mode_change(struct cyttsp4 *ts, u8 cmd)
{
    bool cond = !(cmd & CY_MODE_CHANGE);
    dev_vdbg(ts->dev, "%s: cmd=%02X cond=%d\n", __func__, cmd, (int)cond);
    return cond;
}

static void _cyttsp4_change_state(struct cyttsp4 *ts, enum cyttsp4_driver_state new_state)
{
    ts->driver_state= new_state; 
    _cyttsp4_pr_state(ts);	
}

static int _cyttsp4_put_cmd_wait(struct cyttsp4 *ts, u16 ofs,
                                 size_t cmd_len, const void *cmd_buf, unsigned long timeout_ms,
                                 bool (*cond)(struct cyttsp4 *, u8), u8 *retcmd,
                                 int i2c_addr, bool use_subaddr)
{
    enum cyttsp4_driver_state tmp_state;
    unsigned long uretval = 0;
    u8 cmd = 0;
    int tries = 0;
    int retval = 0;

    /* unlock here to allow any pending irq to complete */
    tmp_state = ts->driver_state;
    _cyttsp4_change_state(ts, CY_TRANSFER_STATE);
    if (mutex_is_locked(&ts->data_lock) != 0) {
        mutex_unlock(&ts->data_lock);
    }
    mutex_lock(&ts->data_lock);
    _cyttsp4_change_state(ts, CY_CMD_STATE);
    INIT_COMPLETION(ts->int_running);
    mutex_unlock(&ts->data_lock);
    retval = _cyttsp4_write_block_data(ts, ofs, cmd_len, cmd_buf, i2c_addr, use_subaddr);
    if (retval < 0) {
        dev_err(ts->dev, "%s: Fail writing cmd buf r=%d\n",__func__, retval);
        mutex_lock(&ts->data_lock);
        goto _cyttsp4_put_cmd_wait_exit;
    }
_cyttsp4_put_cmd_wait_retry:
    uretval = wait_for_completion_interruptible_timeout(
                                                        &ts->int_running, msecs_to_jiffies(timeout_ms));
    mutex_lock(&ts->data_lock);

    retval = _cyttsp4_read_block_data(ts, ofs,
                                      sizeof(cmd), &cmd, i2c_addr, use_subaddr);
    if (retval < 0) {
        dev_err(ts->dev, "%s: fail read cmd status  r=%d\n",__func__, retval);
    }
    if ((cond != NULL) && !cond(ts, cmd)) {
        if (uretval == 0) {
            dev_err(ts->dev, "%s: timeout waiting for cmd ready\n",__func__);
            retval = -ETIMEDOUT;
        } else {
            if (tries++ < 2) {
                INIT_COMPLETION(ts->int_running);
                mutex_unlock(&ts->data_lock);
                goto _cyttsp4_put_cmd_wait_retry;
            } else {
                dev_err(ts->dev, "%s: cmd not ready error" " cmd_stat=0x%02X\n",__func__, cmd);
                retval = -EIO;
            }
        }
    } else {
        /* got command ready */
        if (retcmd != NULL)
            *retcmd = cmd;
        retval = 0;
        dev_vdbg(ts->dev, "%s: got command ready; cmd=%02X retcmd=%p tries=%d\n",
                 __func__, cmd, retcmd, tries);
    }

_cyttsp4_put_cmd_wait_exit:
    _cyttsp4_change_state(ts, tmp_state);
    return retval;
}

static int _cyttsp4_handshake(struct cyttsp4 *ts, u8 hst_mode)
{
    int retval = 0;
    u8 cmd = 0;

    cmd = hst_mode & CY_HANDSHAKE_BIT ?
        hst_mode & ~CY_HANDSHAKE_BIT :
        hst_mode | CY_HANDSHAKE_BIT;

    retval = _cyttsp4_write_block_data(ts, CY_REG_BASE,
                                       sizeof(cmd), (u8 *)&cmd,
                                       ts->platform_data->addr[CY_TCH_ADDR_OFS], true);

    if (retval < 0) {
        dev_err(ts->dev, "%s: bus write fail on handshake (ret=%d)\n",__func__, retval);
    }

    return retval;
}

static int _cyttsp4_cmd_handshake(struct cyttsp4 *ts)
{
    u8 host_mode = 0;
    int retval = 0;

    retval = _cyttsp4_read_block_data(ts, CY_REG_BASE,
                                      sizeof(host_mode), &host_mode,
                                      ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
    if (retval < 0) {
        dev_err(ts->dev, "%s: Fail read host mode r=%d\n",__func__, retval);
    } else {
        retval = _cyttsp4_handshake(ts, host_mode);
        if (retval < 0) {
            dev_err(ts->dev, "%s: Fail handshake r=%d\n",__func__, retval);
        }
    }

    return retval;
}

#ifdef CY_USE_TMA400
#if defined(CY_AUTO_LOAD_TOUCH_PARAMS)
static void _cyttsp_read_table_crc(struct cyttsp4 *ts, const u8 *ptable,
	u8 *crc_h, u8 *crc_l)
{
	size_t crc_loc = (ptable[3] * 256) + ptable[2];

	*crc_h = ptable[crc_loc];
	*crc_l = ptable[crc_loc + 1];
}
#endif

/* Get EBID Row Size is a Config mode command */
static int _cyttsp4_get_ebid_row_size(struct cyttsp4 *ts)
{
    int retval = 0;
    u8 cmd = 0;
    u8 cmd_dat[CY_NUM_DAT + 1];	/* +1 for cmd byte */

    memset(cmd_dat, 0, sizeof(cmd_dat));
    cmd_dat[0] = CY_GET_EBID_ROW_SIZE;	/* get EBID row size command */

    retval = _cyttsp4_put_cmd_wait(ts, ts->si_ofs.cmd_ofs,
                                   sizeof(cmd_dat), cmd_dat, CY_HALF_SEC_TMO_MS,
                                   _cyttsp4_chk_cmd_rdy, &cmd,
                                   ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
    if (retval < 0) {
        dev_err(ts->dev, "%s: Fail Get EBID row size command r=%d\n",	__func__, retval);
    } else {
        retval = _cyttsp4_read_block_data(ts, ts->si_ofs.cmd_ofs,
                                          sizeof(cmd_dat), cmd_dat,
                                          ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
        if (retval < 0) {
            dev_err(ts->dev, "%s: Fail get EBID row size r=%d\n",__func__, retval);
            ts->ebid_row_size = CY_EBID_ROW_SIZE_DFLT;
            dev_err(ts->dev, "%s: Use default EBID row size=%d\n",__func__, ts->ebid_row_size);
        } else {
            ts->ebid_row_size = (cmd_dat[1] * 256) + cmd_dat[2];
            retval = _cyttsp4_cmd_handshake(ts);
            if (retval < 0) {
                dev_err(ts->dev, "%s: Command handshake error r=%d\n",	__func__, retval);
                /* continue anyway; rely on handshake tmo */
                retval = 0;
            }
        }
    }

    return retval;
}

#if defined(CY_AUTO_LOAD_DDATA) || \
    defined(CY_AUTO_LOAD_MDATA) || \
    defined(CY_USE_TOUCH_MONITOR) || \
    defined(CONFIG_TOUCHSCREEN_DEBUG)
/* Get EBID Row Data is a Config mode command */
static int _cyttsp4_get_ebid_data_tma400(struct cyttsp4 *ts, enum cyttsp4_ic_ebid ebid, size_t row_id, u8 *pdata)
{
    int rc = 0;
    int retval = 0;
    u8 crc_h = 0;
    u8 crc_l = 0;
    u8 cmd = 0;
    u8 status = 0;
    u8 cmd_dat[CY_NUM_DAT + 1];	/* +1 for cmd byte */

    memset(cmd_dat, 0, sizeof(cmd_dat));
    cmd_dat[0] = CY_READ_EBID_DATA;	/* get EBID data command */
    cmd_dat[1] = row_id / 256;
    cmd_dat[2] = row_id % 256;
    cmd_dat[3] = ts->ebid_row_size / 256;
    cmd_dat[4] = ts->ebid_row_size % 256;
    cmd_dat[5] = ebid;

    if (pdata == NULL) {
        dev_err(ts->dev, "%s: Get EBID=%d row=%d Data buffer err ptr=%p\n",__func__, ebid, row_id, pdata);
        goto _cyttsp4_get_ebid_data_tma400_exit;
    }

    retval = _cyttsp4_put_cmd_wait(ts, ts->si_ofs.cmd_ofs,
                                   sizeof(cmd_dat), cmd_dat, CY_HALF_SEC_TMO_MS,
                                   _cyttsp4_chk_cmd_rdy, &cmd,
                                   ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
    if (retval < 0) {
        dev_err(ts->dev, "%s: Fail Get EBID=%d row=%d Data cmd r=%d\n",	__func__, ebid, row_id, retval);
        goto _cyttsp4_get_ebid_data_tma400_exit;
    }

    retval = _cyttsp4_read_block_data(ts, ts->si_ofs.cmd_ofs + 1,	sizeof(status), &status,ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
    if (retval < 0) {
        dev_err(ts->dev, "%s: Fail get EBID=%d row=%d status r=%d\n",__func__, ebid, row_id, retval);
        goto _cyttsp4_get_ebid_data_tma400_exit;
    }

    if (status != 0x00) {
        dev_err(ts->dev, "%s: Get EBID=%d row=%d status=%d error\n",__func__, ebid, row_id, status);
        retval = -EIO;
        goto _cyttsp4_get_ebid_data_tma400_exit;
    }

    retval = _cyttsp4_read_block_data(ts, ts->si_ofs.cmd_ofs + 1 + 5,
                                      ts->ebid_row_size + 2, pdata,
                                      ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
    if (retval < 0) {
        dev_err(ts->dev, "%s: fail EBID=%d row=%d data r=%d\n",	__func__, ebid, row_id, retval);
        retval = -EIO;
    } else {
        _cyttsp4_calc_crc(ts, pdata, ts->ebid_row_size, &crc_h, &crc_l);
        if (pdata[ts->ebid_row_size] != crc_h ||
            pdata[ts->ebid_row_size + 1] != crc_l) {
            dev_err(ts->dev, "%s: EBID=%d row_id=%d row_data_crc=%02X%02X" " not equal to calc_crc=%02X%02X\n",__func__, ebid, row_id,
                    pdata[ts->ebid_row_size],
                    pdata[ts->ebid_row_size + 1],
                    crc_h, crc_l);
            /* continue anyway; allow handshake */
            rc = -EIO;
        }
        retval = _cyttsp4_cmd_handshake(ts);
        if (retval < 0) {
            dev_err(ts->dev, "%s: Command handshake error r=%d\n",	__func__, retval);
            /* continue anyway; rely on handshake tmo */
            retval = 0;
        }
        retval = rc;
    }

_cyttsp4_get_ebid_data_tma400_exit:
    return retval;
}
#endif /* --CY_AUTO_LOAD_DDATA || CY_AUTO_LOAD_MDATA || CONFIG_TOUCHSCREEN_DEBUG || CY_USE_TOUCH_MONITOR */

#if defined(CY_AUTO_LOAD_TOUCH_PARAMS) || \
	defined(CY_AUTO_LOAD_DDATA) || \
    defined(CY_AUTO_LOAD_MDATA) || \
	defined(CY_USE_DEV_DEBUG_TOOLS) || \
    defined(CY_USE_TOUCH_MONITOR)
/* Put EBID Row Data is a Config mode command */
static int _cyttsp4_put_ebid_data_tma400(struct cyttsp4 *ts,
                                         enum cyttsp4_ic_ebid ebid, size_t row_id, u8 *out_data)
{
    u8 calc_crc[2];
    u8 *pdata = NULL;
    u8 ret_cmd = 0;
    size_t psize = 0;
    u8 status = 0;
    int retval = 0;

    memset(calc_crc, 0, sizeof(calc_crc));
    psize = 1 + 5 + ts->ebid_row_size + sizeof(cyttsp4_security_key) + 2;
    pdata = kzalloc(psize, GFP_KERNEL);
    if (pdata == NULL || out_data == NULL) {
        dev_err(ts->dev, "%s: Buffer ptr err EBID=%d row=%d" " alloc_ptr=%p out_data=%p\n",__func__, ebid, row_id, pdata, out_data);
        retval = -EINVAL;
    } else {
        pdata[0] = CY_WRITE_EBID_DATA;	/* put ebid data command */
        pdata[1] = row_id / 256;
        pdata[2] = row_id % 256;
        pdata[3] = ts->ebid_row_size / 256;
        pdata[4] = ts->ebid_row_size % 256;
        pdata[5] = ebid;
        memcpy(&pdata[1 + 5], out_data, ts->ebid_row_size);
        memcpy(&pdata[1 + 5 + ts->ebid_row_size],
               cyttsp4_security_key, sizeof(cyttsp4_security_key));
        _cyttsp4_calc_crc(ts, &pdata[1 + 5], ts->ebid_row_size,
                          &calc_crc[0], &calc_crc[1]);
        memcpy(&pdata[1 + 5 + ts->ebid_row_size +
               sizeof(cyttsp4_security_key)],
               calc_crc, sizeof(calc_crc));

        retval = _cyttsp4_put_cmd_wait(ts, ts->si_ofs.cmd_ofs,
                                       psize, pdata, CY_HALF_SEC_TMO_MS,
                                       _cyttsp4_chk_cmd_rdy, &ret_cmd,
                                       ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
        if (retval < 0) {
            dev_err(ts->dev, "%s: Fail Put EBID=%d row=%d Data cmd r=%d\n",	__func__, ebid, row_id, retval);
            goto _cyttsp4_put_ebid_data_tma400_exit;
        }

        retval = _cyttsp4_read_block_data(ts, ts->si_ofs.cmd_ofs + 1,
                                          sizeof(status), &status,
                                          ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
        if (retval < 0) {
            dev_err(ts->dev, "%s: Fail put EBID=%d row=%d"	" read status r=%d\n",__func__, ebid, row_id, retval);
            goto _cyttsp4_put_ebid_data_tma400_exit;
        }

        retval = _cyttsp4_cmd_handshake(ts);
        if (retval < 0) {
            dev_err(ts->dev, "%s: Fail handshake on Put EBID=%d row=%d"	" r=%d\n", __func__, ebid, row_id, retval);
            /* continue; rely on handshake tmo */
            retval = 0;
        }

        if (status != 0x00) {
            dev_err(ts->dev,
                    "%s: Put EBID=%d row=%d status=%d error\n",
                    __func__, ebid, row_id, status);
            retval = -EIO;
        } else
            retval = 0;
    }
_cyttsp4_put_ebid_data_tma400_exit:
    if (pdata != NULL)
        kfree(pdata);
    return retval;
}
#endif /* --CY_AUTO_LOAD_TOUCH_PARAMS || CY_AUTO_LOAD_DDATA || CY_AUTO_LOAD_MDATA || CY_USE_DEV_DEBUG_TOOLS*/

#if defined(CY_AUTO_LOAD_TOUCH_PARAMS) || \
    (defined(CONFIG_TOUCH_SCREEN_DEBUG) && defined(CY_USE_TMA400))
/* Put All Touch Params is a Config mode command */
static int _cyttsp4_put_all_params_tma400(struct cyttsp4 *ts)
{
    enum cyttsp4_ic_ebid ebid = CY_TCH_PARM_EBID;
    size_t row_id = 0;
    size_t num_rows = 0;
    size_t table_size = 0;
    size_t residue = 0;
    u8 *pdata = NULL;
    u8 *ptable = NULL;
    int retval = 0;

    pdata = kzalloc(ts->ebid_row_size, GFP_KERNEL);
    if (pdata == NULL) {
        dev_err(ts->dev, "%s: Alloc error ebid=%d\n",__func__, ebid);
        retval = -ENOMEM;
    } else if (ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL] == NULL)
        dev_err(ts->dev, "%s: NULL param values table\n", __func__);
    else if (ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL]->data == NULL)
        dev_err(ts->dev, "%s: NULL param values table data\n", __func__);
    else if (ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL]->size == 0)
        dev_err(ts->dev, "%s: param values table size is 0\n", __func__);
    else {
        ptable = (u8 *)ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL]->data;
        table_size = ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL]->size;
        num_rows = table_size / ts->ebid_row_size;
        dev_vdbg(ts->dev, "%s: num_rows=%d row_size=%d" " table_size=%d\n", __func__,num_rows, ts->ebid_row_size, table_size);
        for (row_id = 0; row_id < num_rows;) {
            memcpy(pdata, ptable, ts->ebid_row_size);
            dev_vdbg(ts->dev, "%s: row=%d pdata=%p\n",__func__, row_id, pdata);
            _cyttsp4_pr_buf(ts, pdata, ts->ebid_row_size,"ebid_data");
            retval = _cyttsp4_put_ebid_data_tma400(ts, ebid, row_id, pdata);
            if (retval < 0) {
                dev_err(ts->dev, "%s: Fail put row=%d r=%d\n",__func__, row_id, retval);
                break;
            } else {
                ptable += ts->ebid_row_size;
                row_id++;
            }
        }
        if (!(retval < 0)) {
            residue = table_size % ts->ebid_row_size;
            if (residue) {
                memset(pdata, 0, ts->ebid_row_size);
                memcpy(pdata, ptable, residue);
                dev_vdbg(ts->dev, "%s: ebid=%d row=%d data:\n",__func__, ebid, row_id);
                _cyttsp4_pr_buf(ts, pdata, ts->ebid_row_size,"ebid_data");
                retval = _cyttsp4_put_ebid_data_tma400(ts,ebid, row_id, pdata);
                if (retval < 0) {
                    dev_err(ts->dev, "%s: Fail put row=%d r=%d\n",__func__, row_id, retval);
                }
            }
        }
    }

    if (pdata != NULL)
        kfree(pdata);

    return retval;
}
#endif /* --CY_AUTO_LOAD_TOUCH_PARAMS) || CY_USE_DEV_DEBUG_TOOLS */

#ifdef CY_USE_MDDATA_UPDATE
/* Check MDDATA is a Config mode command */
static int _cyttsp4_check_mddata_tma400(struct cyttsp4 *ts, bool *updated)
{
    bool ddata_updated = false;
    bool mdata_updated = false;
#if defined(CY_AUTO_LOAD_DDATA) || defined(CY_AUTO_LOAD_MDATA)
    enum cyttsp4_ic_ebid ebid = CY_DDATA_EBID;
    size_t num_data = 0;
    size_t crc_ofs = 0;
    u8 crc_h = 0;
    u8 crc_l = 0;
#endif
    u8 *pdata = NULL;
    u8 *pmddata = NULL;
    int retval = 0;

    if (ts->ebid_row_size == 0) {
        dev_err(ts->dev, "%s: fail allocate set MDDATA buffer\n", __func__);
        retval = -EINVAL;
        goto _cyttsp4_check_mddata_tma400_exit;
    }
    pdata = kzalloc(ts->ebid_row_size, GFP_KERNEL);
    if (pdata == NULL) {
        dev_err(ts->dev, "%s: fail allocate set MDDATA buffer\n", __func__);
        retval = -ENOMEM;
        goto _cyttsp4_check_mddata_tma400_exit;
    }
    pmddata = kzalloc(ts->ebid_row_size, GFP_KERNEL);
    if (pmddata == NULL) {
        dev_err(ts->dev, "%s: fail allocate set MDDATA buffer\n", __func__);
        retval = -ENOMEM;
        goto _cyttsp4_check_mddata_tma400_exit;
    }

#ifdef CY_AUTO_LOAD_DDATA
    /* check for platform_data DDATA */
    ebid = CY_DDATA_EBID;
    if (ts->platform_data->sett[CY_IC_GRPNUM_DDATA_REC] == NULL) {
        dev_vdbg(ts->dev, "%s: No platform DDATA table\n", __func__);
        goto _cyttsp4_check_mdata_block;
    }
    if (ts->platform_data->sett[CY_IC_GRPNUM_DDATA_REC]->data == NULL) {
        dev_vdbg(ts->dev, "%s: No platform DDATA table data\n", __func__);
        goto _cyttsp4_check_mdata_block;
    }
    if (ts->platform_data->sett[CY_IC_GRPNUM_DDATA_REC]->size == 0) {
        dev_vdbg(ts->dev, "%s: Platform DDATA table has size=0\n", __func__);
        goto _cyttsp4_check_mdata_block;
    }

    dev_vdbg(ts->dev,"%s: call get ebid data for DDATA\n", __func__);
    retval = _cyttsp4_get_ebid_data_tma400(ts, ebid, 0, pdata);
    if (retval < 0) {
        dev_err(ts->dev,	"%s: Fail get DDATA r=%d\n", __func__, retval);
        goto _cyttsp4_check_mdata_block;
    }

    dev_vdbg(ts->dev,"%s: copy pdata -> pmddata\n", __func__);
    memcpy(pmddata, pdata, 4);
    num_data = ts->platform_data->sett
        [CY_IC_GRPNUM_DDATA_REC]->size < CY_NUM_DDATA ?
        ts->platform_data->sett
        [CY_IC_GRPNUM_DDATA_REC]->size : CY_NUM_DDATA;
    dev_vdbg(ts->dev,"%s: copy %d bytes from platform data to ddata array\n",__func__, num_data);
    memcpy(&pmddata[4], ts->platform_data->sett[CY_IC_GRPNUM_DDATA_REC]->data, num_data);
    if (num_data < CY_NUM_DDATA)
        memset(&pmddata[4 + num_data], 0, CY_NUM_DDATA - num_data);
    crc_ofs = (pmddata[3] * 256) + pmddata[2];
    if (crc_ofs == 0)
        crc_ofs = 126;
    dev_vdbg(ts->dev,"%s: ddata crc_ofs=%d num_data=%d\n",__func__, crc_ofs, num_data);

    _cyttsp4_calc_crc(ts, pmddata, crc_ofs, &crc_h, &crc_l);
    pmddata[crc_ofs] = crc_l;
    pmddata[crc_ofs+1] = crc_h;
    _cyttsp4_pr_buf(ts, pdata, ts->ebid_row_size, "pdata");
    _cyttsp4_pr_buf(ts, pmddata, ts->ebid_row_size, "pmddata");
    if (pmddata[crc_ofs] != pdata[crc_ofs] ||
        pmddata[crc_ofs+1] != pdata[crc_ofs+1]) {
        retval = _cyttsp4_put_ebid_data_tma400(ts, ebid, 0, pmddata);
        if (retval < 0)
            dev_err(ts->dev,	"%s: Fail put DDATA r=%d\n", __func__, retval);
        else
            ddata_updated = true;
    }

_cyttsp4_check_mdata_block:
#else
    ddata_updated = false;
#endif /* --CY_AUTO_LOAD_DDATA */

#ifdef CY_AUTO_LOAD_MDATA
    /* check for platform_data MDATA */
    memset(pdata, 0, ts->ebid_row_size);
    memset(pmddata, 0, ts->ebid_row_size);
    ebid = CY_MDATA_EBID;
    if (ts->platform_data->sett[CY_IC_GRPNUM_MDATA_REC] == NULL) {
        dev_vdbg(ts->dev,"%s: No platform MDATA table\n", __func__);
        goto _cyttsp4_check_mddata_tma400_exit;
    }
    if (ts->platform_data->sett[CY_IC_GRPNUM_MDATA_REC]->data == NULL) {
        dev_vdbg(ts->dev,"%s: No platform MDATA table data\n", __func__);
        goto _cyttsp4_check_mddata_tma400_exit;
    }
    if (ts->platform_data->sett[CY_IC_GRPNUM_MDATA_REC]->size == 0) {
        dev_vdbg(ts->dev,"%s: Platform MDATA table has size=0\n", __func__);
        goto _cyttsp4_check_mddata_tma400_exit;
    }

    retval = _cyttsp4_get_ebid_data_tma400(ts, ebid, 0, pdata);
    if (retval < 0) {
        dev_err(ts->dev,	"%s: Fail get MDATA r=%d\n", __func__, retval);
        goto _cyttsp4_check_mddata_tma400_exit;
    }

    memcpy(pmddata, pdata, 4);
    num_data = ts->platform_data->sett
        [CY_IC_GRPNUM_MDATA_REC]->size < CY_NUM_MDATA ?
        ts->platform_data->sett
        [CY_IC_GRPNUM_MDATA_REC]->size : CY_NUM_MDATA;
    dev_vdbg(ts->dev,"%s: copy %d bytes from platform data to mdata array\n",__func__, num_data);
    memcpy(&pmddata[4], ts->platform_data->sett
           [CY_IC_GRPNUM_MDATA_REC]->data, num_data);
    if (num_data < CY_NUM_MDATA)
        memset(&pmddata[4 + num_data], 0, CY_NUM_MDATA - num_data);
    crc_ofs = (pmddata[3] * 256) + pmddata[2];
    if (crc_ofs == 0)
        crc_ofs = 124;
    dev_vdbg(ts->dev,"%s: mdata crc_ofs=%d num_data=%d\n",__func__, crc_ofs, num_data);
    _cyttsp4_calc_crc(ts, pmddata, crc_ofs, &crc_h, &crc_l);
    pmddata[crc_ofs] = crc_l;
    pmddata[crc_ofs+1] = crc_h;
    _cyttsp4_pr_buf(ts, pdata, ts->ebid_row_size, "pdata");
    _cyttsp4_pr_buf(ts, pmddata, ts->ebid_row_size, "pmddata");
    if (pmddata[crc_ofs] != pdata[crc_ofs] ||
        pmddata[crc_ofs+1] != pdata[crc_ofs+1]) {
        retval = _cyttsp4_put_ebid_data_tma400(ts, ebid, 0, pmddata);
        if (retval < 0)
            dev_err(ts->dev,	"%s: Fail put MDATA r=%d\n", __func__, retval);
        else
            mdata_updated = true;
    }
#else
    mdata_updated = false;
#endif /* --CY_AUTO_LOAD_MDATA */

_cyttsp4_check_mddata_tma400_exit:
    if (pdata != NULL)
        kfree(pdata);
    if (pmddata != NULL)
        kfree(pmddata);
    if (updated != NULL)
        *updated = ddata_updated || mdata_updated;
    return retval;
}
#endif /* --CY_USE_MDDATA_UPDATE */
#endif /* --CY_USE_TMA400 */

#ifdef CY_USE_TMA884
static int _cyttsp4_handshake_enable(struct cyttsp4 *ts)
{
	int retval = 0;
	u8 cmd_dat[CY_NUM_DAT + 1];	/* +1 for cmd byte */

	memset(cmd_dat, 0, sizeof(cmd_dat));
	cmd_dat[0] = 0x26;	/* handshake enable operational cmd */
	cmd_dat[1] = 0x03;	/* synchronous level handshake */
	retval = _cyttsp4_put_cmd_wait(ts, ts->si_ofs.cmd_ofs,
		sizeof(cmd_dat), cmd_dat, CY_HALF_SEC_TMO_MS,
		_cyttsp4_chk_cmd_rdy, NULL,
		ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: Fail Enable Handshake command r=%d\n",
			__func__, retval);
		goto _cyttsp4_set_handshake_enable_exit;
	}

	retval = _cyttsp4_read_block_data(ts, ts->si_ofs.cmd_ofs,
		sizeof(cmd_dat), cmd_dat,
		ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: Fail read Enable Hanshake command status"
			"r=%d\n", __func__, retval);
		goto _cyttsp4_set_handshake_enable_exit;
	}

	if (cmd_dat[6] != cmd_dat[1]) {
		dev_err(ts->dev,
			"%s: Fail enable handshake in device\n",
			__func__);
		/* return no error and let driver handshake anyway */
	}

	dev_vdbg(ts->dev,
		"%s: check cmd ready r=%d"
		" cmd[]=%02X %02X %02X %02X %02X %02X %02X\n",
		__func__, retval,
		cmd_dat[0], cmd_dat[1], cmd_dat[2], cmd_dat[3],
		cmd_dat[4], cmd_dat[5], cmd_dat[6]);

_cyttsp4_set_handshake_enable_exit:
	return retval;
}
#endif /* --CY_USE_TMA884 */

/*
 * change device mode - For example, change from
 * system information mode to operating mode
 */
static int _cyttsp4_set_device_mode(struct cyttsp4 *ts,
                                    u8 new_mode, u8 new_cur_mode, char *mode)
{
    u8 cmd = 0;
    int retval = 0;

    cmd = new_mode + CY_MODE_CHANGE;

    retval = _cyttsp4_put_cmd_wait(ts, CY_REG_BASE,
                                   sizeof(cmd), &cmd, CY_TEN_SEC_TMO_MS,
                                   _cyttsp4_chk_mode_change, &cmd,
                                   ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
    if (retval < 0) {
        dev_err(ts->dev,	"%s: Fail Set mode command new_mode=%02X r=%d\n",__func__, new_mode, retval);
        goto _cyttsp4_set_device_mode_exit;
    }

    if (cmd != new_mode) {
        dev_err(ts->dev,	"%s: failed to switch to %s mode\n", __func__, mode);
        retval = -EIO;
    } else {
        ts->current_mode = new_cur_mode;
        retval = _cyttsp4_handshake(ts, cmd);
        if (retval < 0) {
            dev_err(ts->dev,	"%s: Fail handshake r=%d\n", __func__, retval);
            /* continue; rely on handshake tmo */
            retval = 0;
        }
    }
    dev_dbg(ts->dev,"%s: check op ready ret=%d host_mode=%02X\n",__func__, retval, cmd);

_cyttsp4_set_device_mode_exit:
    return retval;
}

static int _cyttsp4_set_mode(struct cyttsp4 *ts, u8 new_mode)
{
    enum cyttsp4_driver_state new_state = CY_TRANSFER_STATE;
    u8 new_cur_mode = CY_MODE_OPERATIONAL;
    char *mode = NULL;
#ifdef CY_USE_TMA400
    unsigned long uretval = 0;
#endif /* --CY_USE_TMA400 */
    int retval = 0;

    switch (new_mode) {
    case CY_OPERATE_MODE:
        new_cur_mode = CY_MODE_OPERATIONAL;
        mode = "operational";
        INIT_COMPLETION(ts->ready_int_running);
        _cyttsp4_change_state(ts, CY_READY_STATE);
        new_state = CY_ACTIVE_STATE;
        break;
    case CY_SYSINFO_MODE:
        new_cur_mode = CY_MODE_SYSINFO;
        mode = "sysinfo";
        new_state = CY_SYSINFO_STATE;
        break;
    case CY_CONFIG_MODE:
        new_cur_mode = CY_MODE_OPERATIONAL;
        mode = "config";
        new_state = ts->driver_state;
        break;
    default:
        dev_err(ts->dev,	"%s: invalid mode change request m=0x%02X\n",__func__, new_mode);
        retval = -EINVAL;
        goto _cyttsp_set_mode_exit;
    }

    retval = _cyttsp4_set_device_mode(ts,	new_mode, new_cur_mode, mode);
    if (retval < 0) {
        dev_err(ts->dev,	"%s: Fail switch to %s mode\n", __func__, mode);
        _cyttsp4_change_state(ts, CY_IDLE_STATE);
    } else {
#ifdef CY_USE_TMA400
        if ((new_mode == CY_OPERATE_MODE) && ts->starting_up){
            uretval = _cyttsp4_wait_ready_int_no_init(ts, CY_HALF_SEC_TMO_MS * 5);
        }
#endif /* --CY_USE_TMA400 */
        _cyttsp4_change_state(ts, new_state);
    }

_cyttsp_set_mode_exit:
    return retval;
}

#ifdef CY_USE_TMA884
static int _cyttsp4_write_config_block(struct cyttsp4 *ts, u8 blockid,
	const u8 *pdata, size_t ndata, u8 crc_h, u8 crc_l, const char *name)
{
	uint8_t *buf = NULL;
	size_t buf_size = 0;
	u8 status = 0;
	int retval = 0;

	/* pre-amble (10) + data (122) + crc (2) + key (8) */
	buf_size = sizeof(uint8_t) * 142;
	buf = kzalloc(buf_size, GFP_KERNEL);
	if (buf == NULL) {
		dev_err(ts->dev,
			"%s: Failed to allocate buffer for %s\n",
			__func__, name);
		retval = -ENOMEM;
		goto _cyttsp4_write_config_block_exit;
	}

	if (pdata == NULL) {
		dev_err(ts->dev,
			"%s: bad data pointer\n", __func__);
		retval = -ENXIO;
		goto _cyttsp4_write_config_block_exit;
	}

	if (ndata > 122) {
		dev_err(ts->dev,
			"%s: %s is too large n=%d size=%d\n",
			__func__, name, ndata, 122);
		retval = -EOVERFLOW;
		goto _cyttsp4_write_config_block_exit;
	}

	/* Set command bytes */
	buf[0] = 0x04; /* cmd */
	buf[1] = 0x00; /* row offset high */
	buf[2] = 0x00; /* row offset low */
	buf[3] = 0x00; /* write block length high */
	buf[4] = 0x80; /* write block length low */
	buf[5] = blockid; /* write block id */
	buf[6] = 0x00; /* num of config bytes + 4 high */
	buf[7] = 0x7E; /* num of config bytes + 4 low */
	buf[8] = 0x00; /* max block size w/o crc high */
	buf[9] = 0x7E; /* max block size w/o crc low */

	/* Copy platform data */
	memcpy(&(buf[10]), pdata, ndata);

	/* Copy block CRC */
	buf[132] = crc_h;
	buf[133] = crc_l;

	/* Set key bytes */
	buf[134] = 0x45;
	buf[135] = 0x63;
	buf[136] = 0x36;
	buf[137] = 0x6F;
	buf[138] = 0x34;
	buf[139] = 0x38;
	buf[140] = 0x73;
	buf[141] = 0x77;

	/* Write config block */
	_cyttsp4_pr_buf(ts, buf, buf_size, name);

	retval = _cyttsp4_write_block_data(ts, ts->si_ofs.cmd_ofs + 1,
		141, &(buf[1]),
		ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: Failed to write config %s r=%d\n",
			__func__, name, retval);
		goto _cyttsp4_write_config_block_exit;
	}

	retval = _cyttsp4_put_cmd_wait(ts, ts->si_ofs.cmd_ofs,
		1, &(buf[0]), CY_TEN_SEC_TMO_MS,
		_cyttsp4_chk_cmd_rdy, NULL,
		ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: Fail write config command r=%d\n",
			__func__, retval);
		goto _cyttsp4_write_config_block_exit;
	}

	retval = _cyttsp4_read_block_data(ts, ts->si_ofs.cmd_ofs + 1,
		sizeof(status), &status,
		ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: Fail read status r=%d\n",
			__func__, retval);
		goto _cyttsp4_write_config_block_exit;
	}

	if (status != 0x00) {
		dev_err(ts->dev,
			"%s: Write config status=%d error\n",
			__func__, status);
		goto _cyttsp4_write_config_block_exit;
	}

_cyttsp4_write_config_block_exit:
	kfree(buf);
	return retval;
}
#endif /* --CY_USE_TMA884 */


#ifdef CY_USE_TMA884
#ifdef CY_AUTO_LOAD_TOUCH_PARAMS
static int _cyttsp4_set_op_params(struct cyttsp4 *ts, u8 crc_h, u8 crc_l)
{
	int retval = 0;

	if (ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL] == NULL) {
		dev_err(ts->dev,
			"%s: Missing Platform Touch Parameter"
			" values table\n", __func__);
		retval = -ENXIO;
		goto _cyttsp4_set_op_params_exit;
	}

	if ((ts->platform_data->sett
		[CY_IC_GRPNUM_TCH_PARM_VAL]->data == NULL) ||
		(ts->platform_data->sett
		[CY_IC_GRPNUM_TCH_PARM_VAL]->size == 0)) {
		dev_err(ts->dev,
			"%s: Missing Platform Touch Parameter"
			" values table data\n", __func__);
		retval = -ENXIO;
		goto _cyttsp4_set_op_params_exit;
	}

	/* Change to Config Mode */
	retval = _cyttsp4_set_mode(ts, CY_CONFIG_MODE);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: Failed to switch to config mode"
			" for touch params\n", __func__);
		goto _cyttsp4_set_op_params_exit;
	}
	retval = _cyttsp4_write_config_block(ts, CY_TCH_PARM_EBID,
		ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL]->data,
		ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL]->size,
		crc_h, crc_l, "platform_touch_param_data");

_cyttsp4_set_op_params_exit:
	return retval;
}
#endif /* --CY_AUTO_LOAD_TOUCH_PARAMS */

static int _cyttsp4_set_data_block(struct cyttsp4 *ts, u8 blkid, u8 *pdata,
	size_t ndata, const char *name, bool force, bool *data_updated)
{
	u8 data_crc[2];
	u8 ic_crc[2];
	int retval = 0;

	memset(data_crc, 0, sizeof(data_crc));
	memset(ic_crc, 0, sizeof(ic_crc));
	*data_updated = false;

	_cyttsp4_pr_buf(ts, pdata, ndata, name);

	dev_vdbg(ts->dev,
		"%s: calc %s crc\n", __func__, name);
	retval = _cyttsp4_calc_data_crc(ts, ndata, pdata,
		&data_crc[0], &data_crc[1],
		name);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: fail calc crc for %s (0x%02X%02X) r=%d\n",
			__func__, name,
			data_crc[0], data_crc[1],
			retval);
		goto _cyttsp_set_data_block_exit;
	}

	dev_vdbg(ts->dev,
		"%s: get ic %s crc\n", __func__, name);
	retval = _cyttsp4_set_mode(ts, CY_OPERATE_MODE);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: Failed to switch to operational mode\n", __func__);
		goto _cyttsp_set_data_block_exit;
	}
	retval = _cyttsp4_get_ic_crc(ts, blkid,
		&ic_crc[0], &ic_crc[1]);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: fail get ic crc for %s (0x%02X%02X) r=%d\n",
			__func__, name,
			ic_crc[0], ic_crc[1],
			retval);
		goto _cyttsp_set_data_block_exit;
	}

	dev_vdbg(ts->dev,
		"%s: %s calc_crc=0x%02X%02X ic_crc=0x%02X%02X\n",
		__func__, name,
		data_crc[0], data_crc[1],
		ic_crc[0], ic_crc[1]);
	if ((data_crc[0] != ic_crc[0]) || (data_crc[1] != ic_crc[1]) || force) {
		/* Change to Config Mode */
		retval = _cyttsp4_set_mode(ts, CY_CONFIG_MODE);
		if (retval < 0) {
			dev_err(ts->dev,
			"%s: Failed to switch to config mode"
				" for sysinfo regs\n", __func__);
			goto _cyttsp_set_data_block_exit;
		}
		retval = _cyttsp4_write_config_block(ts, blkid, pdata,
			ndata, data_crc[0], data_crc[1], name);
		if (retval < 0) {
			dev_err(ts->dev,
			"%s: fail write %s config block r=%d\n",
				__func__, name, retval);
			goto _cyttsp_set_data_block_exit;
		}

		dev_vdbg(ts->dev,
			"%s: write %s config block ok\n", __func__, name);
		*data_updated = true;
	}

_cyttsp_set_data_block_exit:
	return retval;
}

static int _cyttsp4_set_sysinfo_regs(struct cyttsp4 *ts, bool *updated)
{
	bool ddata_updated = false;
	bool mdata_updated = false;
#if defined(CY_AUTO_LOAD_DDATA) || defined(CY_AUTO_LOAD_MDATA)
	size_t num_data = 0;
	u8 *pdata = NULL;
	int retval = 0;

	pdata = kzalloc(CY_NUM_MDATA, GFP_KERNEL);
	if (pdata == NULL) {
		dev_err(ts->dev,
			"%s: fail allocate set sysinfo regs buffer\n",
			__func__);
		retval = -ENOMEM;
		goto _cyttsp4_set_sysinfo_regs_err;
	}

#ifdef CY_AUTO_LOAD_DDATA
	/* check for missing DDATA */
	if (ts->platform_data->sett[CY_IC_GRPNUM_DDATA_REC] == NULL) {
		dev_vdbg(ts->dev,
			"%s: No platform_ddata table\n", __func__);
		dev_vdbg(ts->dev,
			"%s: Use a zero filled array to compare with device\n",
			__func__);
		goto _cyttsp4_set_sysinfo_regs_set_ddata_block;
	}
	if ((ts->platform_data->sett[CY_IC_GRPNUM_DDATA_REC]->data == NULL) ||
		(ts->platform_data->sett[CY_IC_GRPNUM_DDATA_REC]->size == 0)) {
		dev_vdbg(ts->dev,
			"%s: No platform_ddata table data\n", __func__);
		dev_vdbg(ts->dev,
			"%s: Use a zero filled array to compare with device\n",
			__func__);
		goto _cyttsp4_set_sysinfo_regs_set_ddata_block;
	}

	/* copy platform data design data to the device eeprom */
	num_data = ts->platform_data->sett
		[CY_IC_GRPNUM_DDATA_REC]->size < CY_NUM_DDATA ?
		ts->platform_data->sett
		[CY_IC_GRPNUM_DDATA_REC]->size : CY_NUM_DDATA;
	dev_vdbg(ts->dev,
		"%s: copy %d bytes from platform data to ddata array\n",
		__func__, num_data);
	memcpy(pdata, ts->platform_data->sett[CY_IC_GRPNUM_DDATA_REC]->data,
		num_data);

_cyttsp4_set_sysinfo_regs_set_ddata_block:
	/* set data block will check CRC match/nomatch */
	retval = _cyttsp4_set_data_block(ts, CY_DDATA_EBID, pdata,
		CY_NUM_DDATA, "platform_ddata", false, &ddata_updated);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: Fail while writing platform_ddata"
			" block to ic r=%d\n", __func__, retval);
	}
#else
	ddata_updated = false;
#endif /* --CY_AUTO_LOAD_DDATA */

#ifdef CY_AUTO_LOAD_MDATA
	/* check for missing MDATA */
	if (ts->platform_data->sett[CY_IC_GRPNUM_MDATA_REC] == NULL) {
		dev_vdbg(ts->dev,
			"%s: No platform_mdata table\n", __func__);
		dev_vdbg(ts->dev,
			"%s: Use a zero filled array to compare with device\n",
			__func__);
		goto _cyttsp4_set_sysinfo_regs_set_mdata_block;
	}
	if ((ts->platform_data->sett[CY_IC_GRPNUM_MDATA_REC]->data == NULL) ||
		(ts->platform_data->sett[CY_IC_GRPNUM_MDATA_REC]->size == 0)) {
		dev_vdbg(ts->dev,
			"%s: No platform_mdata table data\n", __func__);
		dev_vdbg(ts->dev,
			"%s: Use a zero filled array to compare with device\n",
			__func__);
		goto _cyttsp4_set_sysinfo_regs_set_mdata_block;
	}

	/* copy platform manufacturing data to the device eeprom */
	num_data = ts->platform_data->sett
		[CY_IC_GRPNUM_MDATA_REC]->size < CY_NUM_MDATA ?
		ts->platform_data->sett
		[CY_IC_GRPNUM_MDATA_REC]->size : CY_NUM_MDATA;
	dev_vdbg(ts->dev,
		"%s: copy %d bytes from platform data to mdata array\n",
		__func__, num_data);
	memcpy(pdata, ts->platform_data->sett[CY_IC_GRPNUM_MDATA_REC]->data,
		num_data);

_cyttsp4_set_sysinfo_regs_set_mdata_block:
	/* set data block will check CRC match/nomatch */
	retval = _cyttsp4_set_data_block(ts, CY_MDATA_EBID, pdata,
		CY_NUM_MDATA, "platform_mdata", false, &mdata_updated);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: Fail while writing platform_mdata"
			" block to ic r=%d\n", __func__, retval);
	}
#else
	mdata_updated = false;
#endif /* --CY_AUTO_LOAD_MDATA */

	kfree(pdata);
#endif /* --CY_AUTO_LOAD_DDATA || --CY_AUTO_LOAD_DDATA */
_cyttsp4_set_sysinfo_regs_err:
	*updated = ddata_updated || mdata_updated;
	return retval;
}
#endif /* --CY_USE_TMA884 */

static int _cyttsp4_bits_2_bytes(struct cyttsp4 *ts, int nbits, int *max)
{
    int nbytes;

    *max = 1 << nbits;

    for (nbytes = 0; nbits > 0;) {
        dev_vdbg(ts->dev,"%s: nbytes=%d nbits=%d\n", __func__, nbytes, nbits);
        nbytes++;
        if (nbits > 8)
            nbits -= 8;
        else
            nbits = 0;
        dev_vdbg(ts->dev,"%s: nbytes=%d nbits=%d\n", __func__, nbytes, nbits);
    }

    return nbytes;
}

static void _cyttsp4_free_sysinfo_ptr(struct cyttsp4 *ts)
{
    if(ts->sysinfo_ptr.cydata != NULL)
        kfree(ts->sysinfo_ptr.cydata);
    if(ts->sysinfo_ptr.test != NULL)
        kfree(ts->sysinfo_ptr.test);
    if(ts->sysinfo_ptr.pcfg != NULL)
        kfree(ts->sysinfo_ptr.pcfg);
    if(ts->sysinfo_ptr.opcfg != NULL)
        kfree(ts->sysinfo_ptr.opcfg);
    if(ts->sysinfo_ptr.ddata != NULL)
        kfree(ts->sysinfo_ptr.ddata);
    if(ts->sysinfo_ptr.mdata != NULL)
        kfree(ts->sysinfo_ptr.mdata);
    if(ts->btn != NULL)
        kfree(ts->btn);
}

static int _cyttsp4_get_sysinfo_regs(struct cyttsp4 *ts)
{
#ifdef CY_USE_BUTTON
	int btn = 0;
	int num_defined_keys = 0;
	u16 *key_table = NULL;
#endif /* --CY_USE_BUTTON */
	enum cyttsp4_tch_abs abs = 0;
#ifdef CY_USE_TMA400_SP2
#ifdef CY_USE_TMA400
	int i = 0;
#endif /* --CY_USE_TMA400 */
#endif /* --CY_USE_TMA400_SP2 */
	int retval = 0;

	/* pre-clear si_ofs structure */
	memset(&ts->si_ofs, 0, sizeof(struct cyttsp4_sysinfo_ofs));

	/* get the sysinfo data offsets */
	retval = _cyttsp4_read_block_data(ts, CY_REG_BASE,
		sizeof(ts->sysinfo_data), &(ts->sysinfo_data),
		ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: fail read sysinfo data offsets r=%d\n",
			__func__, retval);
		goto _cyttsp4_get_sysinfo_regs_exit_no_handshake;
	} else {
		/* Print sysinfo data offsets */
		_cyttsp4_pr_buf(ts, (u8 *)&ts->sysinfo_data,
			sizeof(ts->sysinfo_data), "sysinfo_data_offsets");

		/* convert sysinfo data offset bytes into integers */
		ts->si_ofs.map_sz = (ts->sysinfo_data.map_szh * 256) +
			ts->sysinfo_data.map_szl;
		ts->si_ofs.cydata_ofs = (ts->sysinfo_data.cydata_ofsh * 256) +
			ts->sysinfo_data.cydata_ofsl;
		ts->si_ofs.test_ofs = (ts->sysinfo_data.test_ofsh * 256) +
			ts->sysinfo_data.test_ofsl;
		ts->si_ofs.pcfg_ofs = (ts->sysinfo_data.pcfg_ofsh * 256) +
			ts->sysinfo_data.pcfg_ofsl;
		ts->si_ofs.opcfg_ofs = (ts->sysinfo_data.opcfg_ofsh * 256) +
			ts->sysinfo_data.opcfg_ofsl;
		ts->si_ofs.ddata_ofs = (ts->sysinfo_data.ddata_ofsh * 256) +
			ts->sysinfo_data.ddata_ofsl;
		ts->si_ofs.mdata_ofs = (ts->sysinfo_data.mdata_ofsh * 256) +
			ts->sysinfo_data.mdata_ofsl;
	}

	/* get the sysinfo cydata */
	ts->si_ofs.cydata_size = ts->si_ofs.test_ofs - ts->si_ofs.cydata_ofs;
	if(ts->si_ofs.cydata_size < 0 || ts->si_ofs.cydata_size > MAX_DATA_SIZE)
	{
		dev_err(ts->dev, "ts->si_ofs.cydata_size: %d\n",ts->si_ofs.cydata_size);
		retval = -ENOMEM;
		dev_err(ts->dev,	"%s[%d]: fail alloc memory r=%d\n",__func__,__LINE__, retval);
		goto _cyttsp4_get_sysinfo_regs_exit_no_handshake;
	}
    ts->sysinfo_ptr.cydata = kzalloc(ts->si_ofs.cydata_size, GFP_KERNEL);
    if (ts->sysinfo_ptr.cydata == NULL) {
        retval = -ENOMEM;
        dev_err(ts->dev,
            "%s: fail alloc cydata memory r=%d\n",
            __func__, retval);
        goto _cyttsp4_get_sysinfo_regs_exit_no_handshake;
	} else {
		memset(ts->sysinfo_ptr.cydata, 0, ts->si_ofs.cydata_size);
		retval = _cyttsp4_read_block_data(ts, ts->si_ofs.cydata_ofs,
			ts->si_ofs.cydata_size, ts->sysinfo_ptr.cydata,
			ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
		if (retval < 0) {
			dev_err(ts->dev,
			"%s: fail read cydata r=%d\n",
				__func__, retval);
			goto _cyttsp4_get_sysinfo_regs_exit;
		}
		/* Print sysinfo cydata */
		_cyttsp4_pr_buf(ts, (u8 *)ts->sysinfo_ptr.cydata,
			ts->si_ofs.cydata_size, "sysinfo_cydata");
	}

	/* get the sysinfo test data */
	ts->si_ofs.test_size = ts->si_ofs.pcfg_ofs - ts->si_ofs.test_ofs;
	if(ts->si_ofs.test_size < 0 || ts->si_ofs.test_size > MAX_DATA_SIZE)
	{
		dev_err(ts->dev, "ts->si_ofs.test_size: %d\n",ts->si_ofs.test_size);
		retval = -ENOMEM;
		dev_err(ts->dev,	"%s[%d]: fail alloc memory r=%d\n",__func__,__LINE__, retval);
		goto _cyttsp4_get_sysinfo_regs_exit_no_handshake;
	}
    ts->sysinfo_ptr.test = kzalloc(ts->si_ofs.test_size, GFP_KERNEL);
    if (ts->sysinfo_ptr.test == NULL) {
        retval = -ENOMEM;
        dev_err(ts->dev,
            "%s: fail alloc test memory r=%d\n",
            __func__, retval);
        goto _cyttsp4_get_sysinfo_regs_exit;
	} else {
		memset(ts->sysinfo_ptr.test, 0, ts->si_ofs.test_size);
		retval = _cyttsp4_read_block_data(ts, ts->si_ofs.test_ofs,
			ts->si_ofs.test_size, ts->sysinfo_ptr.test,
			ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
		if (retval < 0) {
			dev_err(ts->dev,
			"%s: fail read test data r=%d\n",
				__func__, retval);
			goto _cyttsp4_get_sysinfo_regs_exit;
		}
		/* Print sysinfo test data */
		_cyttsp4_pr_buf(ts, (u8 *)ts->sysinfo_ptr.test,
			ts->si_ofs.test_size, "sysinfo_test_data");
#ifdef CY_USE_TMA400
		if (ts->sysinfo_ptr.test->post_codel & 0x01) {
			dev_info(ts->dev,
			"%s: Reset was a WATCHDOG RESET codel=%02X\n",
				__func__, ts->sysinfo_ptr.test->post_codel);
		}

		if (!(ts->sysinfo_ptr.test->post_codel & 0x02)) {
			dev_info(ts->dev,
			"%s: Config Data CRC FAIL codel=%02X\n",
				__func__, ts->sysinfo_ptr.test->post_codel);
		}

		if (!(ts->sysinfo_ptr.test->post_codel & 0x04)) {
			dev_info(ts->dev,
			"%s: PANEL TEST FAIL codel=%02X\n",
				__func__, ts->sysinfo_ptr.test->post_codel);
		}

		dev_info(ts->dev,
			"%s: SCANNING is %s codel=%02X\n", __func__,
			ts->sysinfo_ptr.test->post_codel & 0x08 ? "ENABLED" :
			"DISABLED", ts->sysinfo_ptr.test->post_codel);
#endif /* --CY_USE_TMA400 */
	}

	/* get the sysinfo pcfg data */
	ts->si_ofs.pcfg_size = ts->si_ofs.opcfg_ofs - ts->si_ofs.pcfg_ofs;
	if(ts->si_ofs.pcfg_size < 0 || ts->si_ofs.pcfg_size > MAX_DATA_SIZE)
	{
		dev_err(ts->dev, "ts->si_ofs.pcfg_size: %d\n",ts->si_ofs.pcfg_size);
		retval = -ENOMEM;
		dev_err(ts->dev,	"%s[%d]: fail alloc memory r=%d\n",__func__,__LINE__, retval);
		goto _cyttsp4_get_sysinfo_regs_exit_no_handshake;
	}
    ts->sysinfo_ptr.pcfg = kzalloc(ts->si_ofs.pcfg_size, GFP_KERNEL);
    if (ts->sysinfo_ptr.pcfg == NULL) {
        retval = -ENOMEM;
        dev_err(ts->dev,
            "%s: fail alloc pcfg memory r=%d\n",
            __func__, retval);
        goto _cyttsp4_get_sysinfo_regs_exit;
	} else {
		memset(ts->sysinfo_ptr.pcfg, 0, ts->si_ofs.pcfg_size);
		retval = _cyttsp4_read_block_data(ts, ts->si_ofs.pcfg_ofs,
			ts->si_ofs.pcfg_size, ts->sysinfo_ptr.pcfg,
			ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
		if (retval < 0) {
			dev_err(ts->dev,
			"%s: fail read pcfg data r=%d\n",
				__func__, retval);
			goto _cyttsp4_get_sysinfo_regs_exit;
		}
		/* Print sysinfo pcfg data */
		_cyttsp4_pr_buf(ts, (u8 *)ts->sysinfo_ptr.pcfg,
			ts->si_ofs.pcfg_size, "sysinfo_pcfg_data");
	}

	/* get the sysinfo opcfg data */
	ts->si_ofs.opcfg_size = ts->si_ofs.ddata_ofs - ts->si_ofs.opcfg_ofs;
	if(ts->si_ofs.opcfg_size < 0 || ts->si_ofs.opcfg_size > MAX_DATA_SIZE)
	{
		dev_err(ts->dev, "ts->si_ofs.opcfg_size: %d\n",ts->si_ofs.opcfg_size);
		retval = -ENOMEM;
		dev_err(ts->dev,	"%s[%d]: fail alloc memory r=%d\n",__func__,__LINE__, retval);
		goto _cyttsp4_get_sysinfo_regs_exit_no_handshake;
	}
    ts->sysinfo_ptr.opcfg = kzalloc(ts->si_ofs.opcfg_size, GFP_KERNEL);
    if (ts->sysinfo_ptr.opcfg == NULL) {
        retval = -ENOMEM;
        dev_err(ts->dev,
            "%s: fail alloc opcfg memory r=%d\n",
            __func__, retval);
        goto _cyttsp4_get_sysinfo_regs_exit;
	} else {
		memset(ts->sysinfo_ptr.opcfg, 0, ts->si_ofs.opcfg_size);
		retval = _cyttsp4_read_block_data(ts, ts->si_ofs.opcfg_ofs,
			ts->si_ofs.opcfg_size, ts->sysinfo_ptr.opcfg,
			ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
		if (retval < 0) {
			dev_err(ts->dev,
			"%s: fail read opcfg data r=%d\n",
				__func__, retval);
			goto _cyttsp4_get_sysinfo_regs_exit;
		}
		ts->si_ofs.cmd_ofs = ts->sysinfo_ptr.opcfg->cmd_ofs;
		ts->si_ofs.rep_ofs = ts->sysinfo_ptr.opcfg->rep_ofs;
		ts->si_ofs.rep_sz = (ts->sysinfo_ptr.opcfg->rep_szh * 256) +
			ts->sysinfo_ptr.opcfg->rep_szl;
#ifdef CY_USE_BUTTON
		ts->si_ofs.num_btns = ts->sysinfo_ptr.opcfg->num_btns;
		if (ts->si_ofs.num_btns == 0)
			ts->si_ofs.num_btn_regs = 0;
		else {
			ts->si_ofs.num_btn_regs = ts->si_ofs.num_btns /
				CY_NUM_BTN_PER_REG;
			if (ts->si_ofs.num_btns % CY_NUM_BTN_PER_REG)
				ts->si_ofs.num_btn_regs++;
		}
#endif /* --CY_USE_BUTTON */
		ts->si_ofs.tt_stat_ofs = ts->sysinfo_ptr.opcfg->tt_stat_ofs;
		ts->si_ofs.obj_cfg0 = ts->sysinfo_ptr.opcfg->obj_cfg0;
		ts->si_ofs.max_tchs = ts->sysinfo_ptr.opcfg->max_tchs &
			CY_SIZE_FIELD_MASK;
		ts->si_ofs.tch_rec_siz = ts->sysinfo_ptr.opcfg->tch_rec_siz &
			CY_SIZE_FIELD_MASK;

		/* Get the old touch fields */
		for (abs = CY_TCH_X; abs < CY_NUM_OLD_TCH_FIELDS; abs++) {
			ts->si_ofs.tch_abs[abs].ofs =
				ts->sysinfo_ptr.opcfg->tch_rec_old[abs].loc;
			ts->si_ofs.tch_abs[abs].size =
				_cyttsp4_bits_2_bytes(ts,
				ts->sysinfo_ptr.opcfg->tch_rec_old[abs].size &
				CY_SIZE_FIELD_MASK,
				&ts->si_ofs.tch_abs[abs].max);
			ts->si_ofs.tch_abs[abs].bofs =
				(ts->sysinfo_ptr.opcfg->tch_rec_old[abs].size &
				CY_BOFS_MASK) >> CY_BOFS_SHIFT;
			dev_vdbg(ts->dev,
				"%s: tch_rec_%s\n", __func__,
				cyttsp4_tch_abs_string[abs]);
			dev_vdbg(ts->dev,
				"%s:     ofs =%2d\n", __func__,
				ts->si_ofs.tch_abs[abs].ofs);
			dev_vdbg(ts->dev,
				"%s:     siz =%2d\n", __func__,
				ts->si_ofs.tch_abs[abs].size);
			dev_vdbg(ts->dev,
				"%s:     max =%2d\n", __func__,
				ts->si_ofs.tch_abs[abs].max);
			dev_vdbg(ts->dev,
				"%s:     bofs=%2d\n", __func__,
				ts->si_ofs.tch_abs[abs].bofs);
		}

#ifdef CY_USE_TMA400_SP2
#ifdef CY_USE_TMA400
		/* skip over the button fields */

		/* Get the new touch fields */
		for (i = 0; abs < CY_TCH_NUM_ABS; abs++, i++) {
			ts->si_ofs.tch_abs[abs].ofs =
				ts->sysinfo_ptr.opcfg->tch_rec_new[i].loc;
			ts->si_ofs.tch_abs[abs].size =
				_cyttsp4_bits_2_bytes(ts,
				ts->sysinfo_ptr.opcfg->tch_rec_new[i].size &
				CY_SIZE_FIELD_MASK,
				&ts->si_ofs.tch_abs[abs].max);
			ts->si_ofs.tch_abs[abs].bofs =
				(ts->sysinfo_ptr.opcfg->tch_rec_new[i].size &
				CY_BOFS_MASK) >> CY_BOFS_SHIFT;
			dev_vdbg(ts->dev,
				"%s: tch_rec_%s\n", __func__,
				cyttsp4_tch_abs_string[abs]);
			dev_vdbg(ts->dev,
				"%s:     ofs =%2d\n", __func__,
				ts->si_ofs.tch_abs[abs].ofs);
			dev_vdbg(ts->dev,
				"%s:     siz =%2d\n", __func__,
				ts->si_ofs.tch_abs[abs].size);
			dev_vdbg(ts->dev,
				"%s:     max =%2d\n", __func__,
				ts->si_ofs.tch_abs[abs].max);
			dev_vdbg(ts->dev,
				"%s:     bofs=%2d\n", __func__,
				ts->si_ofs.tch_abs[abs].bofs);
		}
#endif /* --CY_USE_TMA400 */
#endif /* --CY_USE_TMA400_SP2 */

		ts->si_ofs.mode_size = ts->si_ofs.tt_stat_ofs + 1;
		ts->si_ofs.data_size = ts->si_ofs.max_tchs *
			ts->sysinfo_ptr.opcfg->tch_rec_siz;
#ifdef CY_USE_BUTTON
		ts->si_ofs.btn_rec_siz = ts->sysinfo_ptr.opcfg->btn_rec_siz;
		ts->si_ofs.btn_diff_ofs = ts->sysinfo_ptr.opcfg->btn_diff_ofs;
		ts->si_ofs.btn_diff_siz = ts->sysinfo_ptr.opcfg->btn_diff_siz;
		if (ts->si_ofs.num_btns)
			ts->si_ofs.mode_size += ts->si_ofs.num_btn_regs;
#endif /* --CY_USE_BUTTON */

		/* Print sysinfo opcfg data */
		_cyttsp4_pr_buf(ts, (u8 *)ts->sysinfo_ptr.opcfg,
			ts->si_ofs.opcfg_size, "sysinfo_opcfg_data");
	}

#ifdef CY_USE_DDATA // unused
	/* get the sysinfo ddata data */
	ts->si_ofs.ddata_size = ts->si_ofs.mdata_ofs - ts->si_ofs.ddata_ofs;
	if(ts->si_ofs.ddata_size < 0 || ts->si_ofs.ddata_size > MAX_DATA_SIZE)
	{
		dev_err(ts->dev, "ts->si_ofs.opcfg_size: %d\n",ts->si_ofs.opcfg_size);
		retval = -ENOMEM;
		dev_err(ts->dev,	"%s[%d]: fail alloc memory r=%d\n",__func__,__LINE__, retval);
		goto _cyttsp4_get_sysinfo_regs_exit_no_handshake;
	}
    ts->sysinfo_ptr.ddata = kzalloc(ts->si_ofs.ddata_size, GFP_KERNEL);
    if (ts->sysinfo_ptr.ddata == NULL) {
        dev_err(ts->dev,
            "%s: fail alloc ddata memory r=%d\n",
            __func__, retval);
        /* continue */
	} else {
		memset(ts->sysinfo_ptr.ddata, 0, ts->si_ofs.ddata_size);
		retval = _cyttsp4_read_block_data(ts, ts->si_ofs.ddata_ofs,
			ts->si_ofs.ddata_size, ts->sysinfo_ptr.ddata,
			ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
		if (retval < 0) {
			dev_err(ts->dev,
			"%s: fail read ddata data r=%d\n",
				__func__, retval);
			goto _cyttsp4_get_sysinfo_regs_exit;
		}
		/* Print sysinfo ddata */
		_cyttsp4_pr_buf(ts, (u8 *)ts->sysinfo_ptr.ddata,
			ts->si_ofs.ddata_size, "sysinfo_ddata");
	}
#endif /* --CY_USE_DDATA */

#ifdef CY_USE_MDATA // unused
	/* get the sysinfo mdata data */
	ts->si_ofs.mdata_size = ts->si_ofs.map_sz - ts->si_ofs.mdata_ofs;
	if(ts->si_ofs.mdata_size < 0 || ts->si_ofs.mdata_size > MAX_DATA_SIZE)
	{
		dev_err(ts->dev, "ts->si_ofs.mdata_size: %d\n",ts->si_ofs.mdata_size);
		retval = -ENOMEM;
		dev_err(ts->dev,	"%s[%d]: fail alloc memory r=%d\n",__func__,__LINE__, retval);
		goto _cyttsp4_get_sysinfo_regs_exit_no_handshake;
	}
    ts->sysinfo_ptr.mdata = kzalloc(ts->si_ofs.mdata_size, GFP_KERNEL);
    if (ts->sysinfo_ptr.mdata == NULL) {
        dev_err(ts->dev,
            "%s: fail alloc mdata memory r=%d\n",
            __func__, retval);
        /* continue */
	} else {
		memset(ts->sysinfo_ptr.mdata, 0, ts->si_ofs.mdata_size);
		retval = _cyttsp4_read_block_data(ts, ts->si_ofs.mdata_ofs,
			ts->si_ofs.mdata_size, ts->sysinfo_ptr.mdata,
			ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
		if (retval < 0) {
			dev_err(ts->dev,
			"%s: fail read mdata data r=%d\n",
				__func__, retval);
			goto _cyttsp4_get_sysinfo_regs_exit;
		}
		/* Print sysinfo mdata */
		_cyttsp4_pr_buf(ts, (u8 *)ts->sysinfo_ptr.mdata,
			ts->si_ofs.mdata_size, "sysinfo_mdata");
	}
#endif /* --CY_USE_MDATA */

#ifdef CY_USE_BUTTON
	if (ts->si_ofs.num_btns) {
		ts->si_ofs.btn_keys_size = ts->si_ofs.num_btns *
			sizeof(struct cyttsp4_btn);
        ts->btn = kzalloc(ts->si_ofs.btn_keys_size, GFP_KERNEL);
        if (ts->btn == NULL) {
            dev_err(ts->dev,
            "%s: fail alloc btn_keys memory r=%d\n",
                __func__, retval);
		} else {
			if (ts->platform_data->sett
				[CY_IC_GRPNUM_BTN_KEYS] == NULL)
				num_defined_keys = 0;
			else if (ts->platform_data->sett
				[CY_IC_GRPNUM_BTN_KEYS]->data == NULL)
				num_defined_keys = 0;
			else
				num_defined_keys = ts->platform_data->sett
					[CY_IC_GRPNUM_BTN_KEYS]->size;
			for (btn = 0; btn < ts->si_ofs.num_btns &&
				btn < num_defined_keys; btn++) {
				key_table = (u16 *)ts->platform_data->sett
					[CY_IC_GRPNUM_BTN_KEYS]->data;
				ts->btn[btn].key_code = key_table[btn];
				ts->btn[btn].enabled = true;
			}
			for (; btn < ts->si_ofs.num_btns; btn++) {
				ts->btn[btn].key_code = KEY_RESERVED;
				ts->btn[btn].enabled = true;
			}
		}
	} else {
		ts->si_ofs.btn_keys_size = 0;
		ts->btn = NULL;
	}
#endif /* --CY_USE_BUTTON */

	dev_vdbg(ts->dev,
		"%s: cydata_ofs =%4d siz=%4d\n", __func__,
		ts->si_ofs.cydata_ofs, ts->si_ofs.cydata_size);
	dev_vdbg(ts->dev,
		"%s: test_ofs   =%4d siz=%4d\n", __func__,
		ts->si_ofs.test_ofs, ts->si_ofs.test_size);
	dev_vdbg(ts->dev,
		"%s: pcfg_ofs   =%4d siz=%4d\n", __func__,
		ts->si_ofs.pcfg_ofs, ts->si_ofs.pcfg_size);
	dev_vdbg(ts->dev,
		"%s: opcfg_ofs  =%4d siz=%4d\n", __func__,
		ts->si_ofs.opcfg_ofs, ts->si_ofs.opcfg_size);
#ifdef CY_USE_DDATA // unused
	dev_vdbg(ts->dev,
		"%s: ddata_ofs  =%4d siz=%4d\n", __func__,
		ts->si_ofs.ddata_ofs, ts->si_ofs.ddata_size);
#endif /* --CY_USE_DDATA */
#ifdef CY_USE_MDATA // unused
	dev_vdbg(ts->dev,
		"%s: mdata_ofs  =%4d siz=%4d\n", __func__,
		ts->si_ofs.mdata_ofs, ts->si_ofs.mdata_size);
#endif /* --CY_USE_MDATA */

	dev_vdbg(ts->dev,
		"%s: cmd_ofs       =%4d\n", __func__, ts->si_ofs.cmd_ofs);
	dev_vdbg(ts->dev,
		"%s: rep_ofs       =%4d\n", __func__, ts->si_ofs.rep_ofs);
	dev_vdbg(ts->dev,
		"%s: rep_sz        =%4d\n", __func__, ts->si_ofs.rep_sz);
#ifdef CY_USE_BUTTON
	dev_vdbg(ts->dev,
		"%s: num_btns      =%4d\n", __func__, ts->si_ofs.num_btns);
	dev_vdbg(ts->dev,
		"%s: num_btn_regs  =%4d\n", __func__, ts->si_ofs.num_btn_regs);
#endif /* --CY_USE_BUTTON */
	dev_vdbg(ts->dev,
		"%s: tt_stat_ofs   =%4d\n", __func__, ts->si_ofs.tt_stat_ofs);
	dev_vdbg(ts->dev,
		"%s: tch_rec_siz   =%4d\n", __func__, ts->si_ofs.tch_rec_siz);
	dev_vdbg(ts->dev,
		"%s: max_tchs      =%4d\n", __func__, ts->si_ofs.max_tchs);
	dev_vdbg(ts->dev,
		"%s: mode_siz      =%4d\n", __func__, ts->si_ofs.mode_size);
	dev_vdbg(ts->dev,
		"%s: data_siz      =%4d\n", __func__, ts->si_ofs.data_size);
	dev_vdbg(ts->dev,
		"%s: map_sz        =%4d\n", __func__, ts->si_ofs.map_sz);

#ifdef CY_USE_BUTTON
	dev_vdbg(ts->dev,
		"%s: btn_rec_siz   =%2d\n", __func__, ts->si_ofs.btn_rec_siz);
	dev_vdbg(ts->dev,
		"%s: btn_diff_ofs  =%2d\n", __func__, ts->si_ofs.btn_diff_ofs);
	dev_vdbg(ts->dev,
		"%s: btn_diff_siz  =%2d\n", __func__, ts->si_ofs.btn_diff_siz);
#endif /* --CY_USE_BUTTON */

	dev_vdbg(ts->dev,
		"%s: mode_size     =%2d\n", __func__, ts->si_ofs.mode_size);
	dev_vdbg(ts->dev,
		"%s: data_size     =%2d\n", __func__, ts->si_ofs.data_size);

	if (ts->xy_mode == NULL)
		ts->xy_mode = kzalloc(ts->si_ofs.mode_size, GFP_KERNEL);
	if (ts->xy_data == NULL)
		ts->xy_data = kzalloc(ts->si_ofs.data_size, GFP_KERNEL);
	if (ts->xy_data_touch1 == NULL) {
		ts->xy_data_touch1 = kzalloc(ts->si_ofs.tch_rec_siz + 1,
			GFP_KERNEL);
	}
#ifdef CY_USE_BUTTON
	if (ts->btn_rec_data == NULL) {
		ts->btn_rec_data = kzalloc(ts->si_ofs.btn_rec_siz *
			ts->si_ofs.num_btns, GFP_KERNEL);
	}
#endif /* --CY_USE_BUTTON */

#ifdef CY_USE_BUTTON
	if(ts->btn_rec_data == NULL) {
		dev_err(ts->dev,
			"%s: fail memory alloc btn_rec_data=%p\n", __func__, ts->btn_rec_data);
		/* continue */
    }
#endif
	if ((ts->xy_mode == NULL) || (ts->xy_data == NULL) ||
		(ts->xy_data_touch1 == NULL)) {
		dev_err(ts->dev,
			"%s: fail memory alloc xy_mode=%p xy_data=%p"
			"xy_data_touch1=%p\n", __func__,
			ts->xy_mode, ts->xy_data,
			ts->xy_data_touch1);
		/* continue */
	}

	dev_vdbg(ts->dev,
		"%s: xy_mode=%p xy_data=%p xy_data_touch1=%p\n",
		__func__, ts->xy_mode, ts->xy_data, ts->xy_data_touch1);

_cyttsp4_get_sysinfo_regs_exit:
	/* provide flow control handshake */
	retval = _cyttsp4_handshake(ts, ts->sysinfo_data.hst_mode);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: handshake fail on sysinfo reg\n",
			__func__);
		/* continue; rely on handshake tmo */
	}

_cyttsp4_get_sysinfo_regs_exit_no_handshake:
    return retval;
}

static int _cyttsp4_load_status_regs(struct cyttsp4 *ts)
{
    int rep_stat_ofs = 0;
    int retval = 0;	

    rep_stat_ofs = ts->si_ofs.rep_ofs + 1;
    if (ts->xy_mode == NULL) {
        dev_err(ts->dev,	"%s: mode ptr not yet initialized xy_mode=%p\n",__func__, ts->xy_mode);
        /* continue */
    } else {
        retval = _cyttsp4_read_block_data(ts, CY_REG_BASE, ts->si_ofs.mode_size, ts->xy_mode,
                                          ts->platform_data->addr[CY_TCH_ADDR_OFS], true);

        if (retval < 0) {
            dev_err(ts->dev,	"%s: fail read mode regs r=%d\n",__func__, retval);
            retval = -EIO;
        }
        _cyttsp4_pr_buf(ts, ts->xy_mode, ts->si_ofs.mode_size, "xy_mode");
    }
    return retval;
}

#ifdef CY_USE_BUTTON
static void _cyttsp4_btn_key_release(struct cyttsp4 *ts,
	int cur_btn, u8 cur_btn_mask, int num_btns)
{
	int btn = 0;

	/* Check for button releases */
	for (btn = 0; btn < num_btns; btn++) {
		if (ts->btn[cur_btn + btn].enabled) {
			switch ((cur_btn_mask >> (btn * CY_BITS_PER_BTN)) &
				(CY_NUM_BTN_EVENT_ID - 1)) {
			case (CY_BTN_RELEASED):
				if (ts->btn[cur_btn + btn].state ==
					CY_BTN_PRESSED) {
					input_report_key(ts->input,
						ts->btn[cur_btn + btn].key_code,
						CY_BTN_RELEASED);
					ts->btn[cur_btn + btn].state =
						CY_BTN_RELEASED;
					input_sync(ts->input);
					dev_dbg(ts->dev,
						"%s: btn=%d key_code=%d"
						" RELEASED\n", __func__,
						cur_btn + btn, ts->btn
						[cur_btn + btn].key_code);
				}
				break;
			case (CY_BTN_PRESSED):
				break;
			default:
				break;
			}
		}
	}
	return;
}

static void _cyttsp4_btn_key_press(struct cyttsp4 *ts,
	int cur_btn, u8 cur_btn_mask, int num_btns)
{
	int btn = 0;

	/* Check for button presses */
	for (btn = 0; btn < num_btns; btn++) {
		if (ts->btn[cur_btn + btn].enabled) {
			switch ((cur_btn_mask >> (btn * CY_BITS_PER_BTN)) &
				(CY_NUM_BTN_EVENT_ID - 1)) {
			case (CY_BTN_RELEASED):
				break;
			case (CY_BTN_PRESSED):
				if (ts->btn[cur_btn + btn].state ==
					CY_BTN_RELEASED) {
					input_report_key(ts->input,
						ts->btn[cur_btn + btn].key_code,
						CY_BTN_PRESSED);
					ts->btn[cur_btn + btn].state =
						CY_BTN_PRESSED;
					input_sync(ts->input);
					dev_dbg(ts->dev,
						"%s: btn=%d key_code=%d"
						" PRESSED\n", __func__,
						cur_btn + btn, ts->btn
						[cur_btn + btn].key_code);
				}
				break;
			default:
				break;
			}
		}
	}
	return;
}
#endif /* --CY_USE_BUTTON */

static void _cyttsp4_get_touch_axis(struct cyttsp4 *ts,
	enum cyttsp4_tch_abs abs, int *axis, int size,
	int max, u8 *xy_data, int bofs)
{
	int nbyte = 0;
	int next = 0;

	for (nbyte = 0, *axis = 0, next = 0; nbyte < size; nbyte++) {
		dev_vdbg(ts->dev,
			"%s: *axis=%02X(%d) size=%d max=%08X xy_data=%p"
			" xy_data[%d]=%02X(%d)\n",
			__func__, *axis, *axis, size, max, xy_data, next,
			xy_data[next], xy_data[next]);
		*axis = (*axis * 256) + (xy_data[next] >> bofs);
		next++;
	}

	*axis &= max - 1;

#ifdef CY_USE_TMA400_SP2
#ifdef CY_USE_TMA400
	/* sign extend signals that can have negative values */
	if (abs == CY_TCH_OR) {
		if (*axis >= (max / 2))
			*axis = -((~(*axis) & (max - 1)) + 1);
	}
#endif /* --CY_USE_TMA400 */
#endif /* --CY_USE_TMA400_SP2 */

	dev_vdbg(ts->dev,
		"%s: *axis=%02X(%d) size=%d max=%08X xy_data=%p"
		" xy_data[%d]=%02X(%d)\n",
		__func__, *axis, *axis, size, max, xy_data, next,
		xy_data[next], xy_data[next]);
}

static void _cyttsp4_get_touch(struct cyttsp4 *ts,
	struct cyttsp4_touch *touch, u8 *xy_data)
{
	enum cyttsp4_tch_abs abs = 0;
#ifdef CY_USE_DEBUG_TOOLS
	int tmp = 0;
	bool flipped = false;
#endif /* --CY_USE_DEBUG_TOOLS */

	for (abs = CY_TCH_X; abs < CY_TCH_NUM_ABS; abs++) {
        if(*cyttsp4_tch_abs_string[abs]==*cyttsp4_tch_abs_string[CY_TCH_E]){
            ts->si_ofs.tch_abs[abs].ofs = ts->si_ofs.tch_abs[CY_TCH_T].ofs;
            ts->si_ofs.tch_abs[abs].bofs = 5; // event id[6:5], touch id[4:0]
        }
		_cyttsp4_get_touch_axis(ts, abs, &touch->abs[abs],
			ts->si_ofs.tch_abs[abs].size,
			ts->si_ofs.tch_abs[abs].max,
			xy_data + ts->si_ofs.tch_abs[abs].ofs,
			ts->si_ofs.tch_abs[abs].bofs);
		dev_vdbg(ts->dev,
			"%s: get %s=%08X(%d) size=%d"
			" ofs=%d max=%d xy_data+ofs=%p bofs=%d\n",
			__func__, cyttsp4_tch_abs_string[abs],
			touch->abs[abs], touch->abs[abs],
			ts->si_ofs.tch_abs[abs].size,
			ts->si_ofs.tch_abs[abs].ofs,
			ts->si_ofs.tch_abs[abs].max,
			xy_data + ts->si_ofs.tch_abs[abs].ofs,
			ts->si_ofs.tch_abs[abs].bofs);
	}

#ifdef CY_USE_DEBUG_TOOLS
	if (ts->flags & CY_FLAG_FLIP) {
		tmp = touch->abs[CY_TCH_X];
		touch->abs[CY_TCH_X] =
			touch->abs[CY_TCH_Y];
		touch->abs[CY_TCH_Y] = tmp;
		flipped = true;
	}
	if (ts->flags & CY_FLAG_INV_X) {
		if (!flipped) {
			touch->abs[CY_TCH_X] =
				ts->platform_data->frmwrk->abs
				[(CY_ABS_X_OST * CY_NUM_ABS_SET) + CY_MAX_OST] -
				touch->abs[CY_TCH_X];
		} else {
			touch->abs[CY_TCH_X] =
				ts->platform_data->frmwrk->abs
				[(CY_ABS_Y_OST * CY_NUM_ABS_SET) + CY_MAX_OST] -
				touch->abs[CY_TCH_X];
		}
	}
	if (ts->flags & CY_FLAG_INV_Y) {
		if (!flipped) {
			touch->abs[CY_TCH_Y] =
				ts->platform_data->frmwrk->abs
				[(CY_ABS_Y_OST * CY_NUM_ABS_SET) + CY_MAX_OST] -
				touch->abs[CY_TCH_Y];
		} else {
			touch->abs[CY_TCH_Y] =
				ts->platform_data->frmwrk->abs
				[(CY_ABS_X_OST * CY_NUM_ABS_SET) + CY_MAX_OST] -
				touch->abs[CY_TCH_Y];
		}
	}
#endif /* --CY_USE_DEBUG_TOOLS */
}

#ifndef CY_USE_PROTOCOL_TYPE_B
static void _cyttsp4_get_mt_touches(struct cyttsp4 *ts, int num_cur_tch)
{
	struct cyttsp4_touch touch;
	int signal = CY_IGNORE_VALUE;
	int i = 0;
	int j = 0;
	int t = 0;

	memset(&touch, 0, sizeof(struct cyttsp4_touch));
	for (i = 0; i < num_cur_tch; i++) {
		_cyttsp4_get_touch(ts, &touch,
			ts->xy_data + (i * ts->si_ofs.tch_rec_siz));

		if ((touch.abs[CY_TCH_T] < ts->platform_data->frmwrk->abs
			[(CY_ABS_ID_OST * CY_NUM_ABS_SET) + CY_MIN_OST]) ||
			(touch.abs[CY_TCH_T] > ts->platform_data->frmwrk->abs
			[(CY_ABS_ID_OST * CY_NUM_ABS_SET) + CY_MAX_OST])) {
			dev_err(ts->dev,
				"%s: touch=%d has bad track_id=%d max_id=%d\n",
				__func__, i, touch.abs[CY_TCH_T],
				ts->platform_data->frmwrk->abs
				[(CY_ABS_ID_OST * CY_NUM_ABS_SET) +
				CY_MAX_OST]);
			input_mt_sync(ts->input);
		} else {
			/* use 0 based track id's */
			signal = ts->platform_data->frmwrk->abs
				[(CY_ABS_ID_OST*CY_NUM_ABS_SET)+0];
			if (signal != CY_IGNORE_VALUE) {
				t = touch.abs[CY_TCH_T] -
					ts->platform_data->frmwrk->abs
					[(CY_ABS_ID_OST * CY_NUM_ABS_SET) +
					CY_MIN_OST];
				input_report_abs(ts->input, signal, t);
			}

			/* all devices: position and pressure fields */
			for (j = 0; j < CY_ABS_W_OST ; j++) {
				signal = ts->platform_data->frmwrk->abs
					[((CY_ABS_X_OST + j) *
					CY_NUM_ABS_SET) + 0];
				if (signal != CY_IGNORE_VALUE) {
					input_report_abs(ts->input, signal,
					touch.abs[CY_TCH_X + j]);
				}
			}

#ifdef CY_USE_TMA884
			/* TMA884 size field */
			signal = ts->platform_data->frmwrk->abs
				[(CY_ABS_W_OST * CY_NUM_ABS_SET) + 0];
			if (signal != CY_IGNORE_VALUE)
				input_report_abs(ts->input,
					signal, touch.abs[CY_TCH_W]);
#endif /* --CY_USE_TMA884 */

#ifdef CY_USE_TMA400_SP2
#ifdef CY_USE_TMA400
			/*
			 * TMA400 size and orientation fields:
			 * if pressure is non-zero and major touch
			 * signal is zero, then set major and minor touch
			 * signal to minimum non-zero value
			 */
			if ((touch.abs[CY_TCH_P] > 0) &&
				(touch.abs[CY_TCH_MAJ] == 0)) {
				touch.abs[CY_TCH_MAJ] = 1;
				touch.abs[CY_TCH_MIN] = 1;
			}

			for (j = 0; j < CY_NUM_NEW_TCH_FIELDS; j++) {
				signal = ts->platform_data->frmwrk->abs
					[((CY_ABS_MAJ_OST + j) *
					CY_NUM_ABS_SET) + 0];
				if (signal != CY_IGNORE_VALUE) {
					input_report_abs(ts->input, signal,
					touch.abs[CY_TCH_MAJ + j]);
				}
			}
#endif /* --CY_USE_TMA400 */
#endif /* --CY_USE_TMA400_SP2 */

			input_mt_sync(ts->input);
		}
#ifdef CY_USE_TMA400_SP2
		dev_dbg(ts->dev,
			"%s: t=%d x=(%d) y=(%d) z=(%d) M=(%d) m=(%d) o=(%d)\n",
			__func__, t,
			touch.abs[CY_TCH_X],
			touch.abs[CY_TCH_Y],
			touch.abs[CY_TCH_P],
			touch.abs[CY_TCH_MAJ],
			touch.abs[CY_TCH_MIN],
			touch.abs[CY_TCH_OR]);
#else
		dev_dbg(ts->dev,
			"%s: t=%d x=(%d) y=(%d) z=(%d)\n", __func__,
			t,
			touch.abs[CY_TCH_X],
			touch.abs[CY_TCH_Y],
			touch.abs[CY_TCH_P]);
#endif /* --CY_USE_TMA400_SP2 */
	}
	input_sync(ts->input);
	ts->num_prv_tch = num_cur_tch;

	return;
}
#endif /* --not use CY_USE_PROTOCOL_TYPE_B */

//++ p11309
static int _pantech_reset_fingerinfo(void)
{
	int num_finger=0;
	int i=0, j=0;

	for (i=0; i<CY_MAX_FINGERNUM; i++) {
		for ( j=0; j<CY_TCH_NUM_ABS; j++) {
			if (( j == CY_TCH_T ) && ( finger_info[i].abs[CY_TCH_T] > -1)) num_finger++;
			finger_info[i].abs[j] = -1;
		}				
	}

	return num_finger;
}

#if 0
static void _pantech_release_input_dev(struct cyttsp4 *ts)
{
	int i=0; 	

	_pantech_reset_fingerinfo();
	input_report_key(ts->input, BTN_TOUCH, 0);
	for (i=0; i<CY_MAX_FINGERNUM; i++) {
		input_mt_slot(ts->input, i);
		input_report_abs(ts->input, ABS_MT_TRACKING_ID, -1);
	}

	input_sync(ts->input);	
}
#endif
//-- p11309

/* read xy_data for all current touches */
static int _cyttsp4_xy_worker(struct cyttsp4 *ts)
{
	struct cyttsp4_touch touch;
	u8 num_cur_tch = 0;
	u8 hst_mode = 0;
	u8 rep_len = 0;
	u8 rep_stat = 0;
	u8 tt_stat = 0;
	int i = 0;
    
//++ p11309 - 2012.09.25 for Preventing from touch_mask crash
	int num_cur_tch_temp = 0;
//-- p11309
    
#ifdef CY_USE_BUTTON
	
	int num_cur_btn = 0;
	int cur_reg = 0;
	u8 cur_btn_mask = 0;
	int cur_btn = 0;
	enum cyttsp4_btn_state btn_state = CY_BTN_RELEASED;
#ifdef CONFIG_TOUCHSCREEN_DEBUG
	int t = 0;
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */
#endif /* --CY_USE_BUTTON */

	int retval = 0;
//++ p11309
	int touch_mask[CY_NUM_TCH_ID];
	for (i=0;i<CY_NUM_TCH_ID;i++) touch_mask[i] = -1;
//-- p11309

	/*
	 * Get event data from CYTTSP device.
	 * The event data includes all data
	 * for all active touches.
	 */
	/*
	 * Use 2 reads: first to get mode bytes,
	 * second to get status (touch count) and touch 1 data.
	 * An optional 3rd read to get touch 2 - touch n data.
	 */
	memset(&touch, 0, sizeof(struct cyttsp4_touch));
	memset(ts->xy_mode, 0, ts->si_ofs.mode_size);
	memset(ts->xy_data_touch1, 0, 1 + ts->si_ofs.tch_rec_siz);

	retval = _cyttsp4_load_status_regs(ts);
	if (retval < 0) {
		/*
		 * bus failure implies Watchdog -> bootloader running
		 * on TMA884 parts
		*/
		dev_err(ts->dev, "%s: 1st read fail on mode regs r=%d\n", __func__, retval);
		retval = -EIO;
		goto _cyttsp4_xy_worker_exit;
	}
	retval = _cyttsp4_read_block_data(ts, ts->si_ofs.tt_stat_ofs,
		1+ts->si_ofs.tch_rec_siz, ts->xy_data_touch1,
		ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
	if (retval < 0) {
		/* bus failure may imply bootloader running */
		dev_err(ts->dev, "%s: read fail on mode regs r=%d\n", __func__, retval);
		retval = -EIO;
		goto _cyttsp4_xy_worker_exit;
	}
#ifdef CONFIG_TOUCHSCREEN_DEBUG
    if (ts->bus_ops->tsdebug >= CY_DBG_LVL_3) {
        printk("%s:", __func__);
        for(retval=0;retval<ts->si_ofs.tch_rec_siz+1;retval++) 
            printk("%02x ", ts->xy_data_touch1[retval]);
        printk("\n");
    }
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */

	hst_mode = ts->xy_mode[CY_REG_BASE];
	rep_len = ts->xy_mode[ts->si_ofs.rep_ofs];
	rep_stat = ts->xy_mode[ts->si_ofs.rep_ofs + 1];
	tt_stat = ts->xy_data_touch1[0];
	dev_dbg(ts->dev,
		"%s: hst_mode=%02X rep_len=%d rep_stat=%02X tt_stat=%02X\n",
		__func__, hst_mode, rep_len, rep_stat, tt_stat);

    if (rep_len == 0) {
        dev_vdbg(ts->dev, "%s: report length error rep_len=%d\n", __func__, rep_len);
        goto _cyttsp4_xy_worker_exit;
    }

	if (GET_NUM_TOUCHES(tt_stat) > 0) {
		memcpy(ts->xy_data, ts->xy_data_touch1 + 1, ts->si_ofs.tch_rec_siz);
	}
	if (GET_NUM_TOUCHES(tt_stat) > 1) {
		retval = _cyttsp4_read_block_data(ts, ts->si_ofs.tt_stat_ofs +
			1 + ts->si_ofs.tch_rec_siz,
			(GET_NUM_TOUCHES(tt_stat) - 1) * ts->si_ofs.tch_rec_siz,
			ts->xy_data + ts->si_ofs.tch_rec_siz,
			ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
		if (retval < 0) {
			dev_err(ts->dev, "%s: read fail on touch regs r=%d\n", __func__, retval);
			goto _cyttsp4_xy_worker_exit;
		}
#ifdef CONFIG_TOUCHSCREEN_DEBUG
        if (ts->bus_ops->tsdebug >= CY_DBG_LVL_3) {
            printk("%s:", __func__);
            for(retval=0;retval<(GET_NUM_TOUCHES(tt_stat) - 1) * ts->si_ofs.tch_rec_siz;retval++) 
                printk("%02x ", ts->xy_data[ts->si_ofs.tch_rec_siz + retval]);
            printk(" (multi touch)\n");
        }
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */
	}

#ifdef CY_USE_BUTTON
#ifdef CONFIG_TOUCHSCREEN_DEBUG
    if (ts->bus_ops->tsdebug >= CY_DBG_LVL_2) {
        if (ts->si_ofs.num_btns > 0) {
            retval = _cyttsp4_read_block_data(ts,
                (ts->si_ofs.tt_stat_ofs + 1) +
                (ts->si_ofs.max_tchs * ts->si_ofs.tch_rec_siz),
                ts->si_ofs.btn_rec_siz * ts->si_ofs.num_btns,
                ts->btn_rec_data,
                ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
            if (retval < 0) {
                dev_err(ts->dev, "%s: read fail on button records r=%d\n", __func__, retval);
                goto _cyttsp4_xy_worker_exit;
            }
            _cyttsp4_pr_buf(ts, ts->btn_rec_data, ts->si_ofs.btn_rec_siz * ts->si_ofs.num_btns, "btn_rec_data");
        }
    }
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */
#endif /* --CY_USE_BUTTON */

	/* provide flow control handshake */
	retval = _cyttsp4_handshake(ts, hst_mode);
	if (retval < 0) {
		dev_err(ts->dev, "%s: handshake fail on operational reg\n", __func__);
		/* continue; rely on handshake tmo */
		retval = 0;
	}

	/* determine number of currently active touches */
	num_cur_tch = GET_NUM_TOUCHES(tt_stat);

	/* print xy data */
	_cyttsp4_pr_buf(ts, ts->xy_data, num_cur_tch * ts->si_ofs.tch_rec_siz, "xy_data");

	/* check for any error conditions */
	if (ts->driver_state == CY_IDLE_STATE) {
        dev_err(ts->dev, "%s: IDLE STATE detected\n", __func__);
		retval = 0;
		goto _cyttsp4_xy_worker_exit;
	} else if (IS_BAD_PKT(rep_stat)) {
        if(printk_ratelimit())
            dev_err(ts->dev, "%s: Invalid buffer detected\n", __func__);
		retval = 0;
		goto _cyttsp4_xy_worker_exit;
	} else if (IS_BOOTLOADERMODE(rep_stat)) {
        if(printk_ratelimit())
            dev_info(ts->dev, "%s: BL mode found in ACTIVE state\n", __func__);
		retval = -EIO;
		goto _cyttsp4_xy_worker_exit;
	} else if (GET_HSTMODE(hst_mode) == GET_HSTMODE(CY_SYSINFO_MODE)) {
		/* if in sysinfo mode switch to op mode */
		dev_err(ts->dev, "%s: Sysinfo mode=0x%02X detected in ACTIVE state\n", __func__, hst_mode);
		retval = _cyttsp4_set_mode(ts, CY_OPERATE_MODE);
		if (retval < 0) {
			_cyttsp4_change_state(ts, CY_IDLE_STATE);
			dev_err(ts->dev, "%s: Fail set operational mode (r=%d)\n", __func__, retval);
		} else {
			_cyttsp4_change_state(ts, CY_ACTIVE_STATE);
			dev_vdbg(ts->dev, "%s: enable handshake\n", __func__);
#ifdef CY_USE_TMA884
			retval = _cyttsp4_handshake_enable(ts);
			if (retval < 0) {
				dev_err(ts->dev, "%s: fail enable handshake r=%d", __func__, retval);
			}
#endif /* --CY_USE_TMA884 */
		}
		goto _cyttsp4_xy_worker_exit;
	} else if (IS_LARGE_AREA(tt_stat)) {
		/* terminate all active tracks */
		num_cur_tch = 0;
		dbg("Large area detected..\n");
	} else if (num_cur_tch > ts->si_ofs.max_tchs) {

//++ p11309 - 2012.09.29 preventing from exceed MAX_FINGER: 10
		if ( num_cur_tch > CY_NUM_TCH_ID ) {			
			dbg("Finger number is over: %d...set to zero\n", num_cur_tch);
			num_cur_tch = 0;
		}
//-- p11309

// 		if (num_cur_tch == 0x1F) {
// 			/* terminate all active tracks */
// 			dev_err(ts->dev, "%s: Num touch err detected (n=%d)\n", __func__, num_cur_tch);
// 			num_cur_tch = 0;
// 		} else {
// 			dev_err(ts->dev, "%s: too many tch; set to max tch (n=%d c=%d)\n",
// 				__func__, num_cur_tch, CY_NUM_TCH_ID);
// 			num_cur_tch = CY_NUM_TCH_ID;
// 		}
	}

	dev_vdbg(ts->dev, "%s: num_prv_tch=%d num_cur_tch=%d\n", __func__, ts->num_prv_tch, num_cur_tch);	

//	printk("[CYTTSP4] FingerNum : %d (%d)\n", num_cur_tch, ts->num_prv_tch);

//++ p11309 - 2012.09.25 for Preventing from touch_mask crash
	num_cur_tch_temp = num_cur_tch;

	for (i=0;i<num_cur_tch;i++) {

		_cyttsp4_get_touch(ts, &touch, ts->xy_data + (i * ts->si_ofs.tch_rec_siz));

		if ( touch.abs[CY_TCH_T] < CY_NUM_TCH_ID ) {
			touch_mask[touch.abs[CY_TCH_T]] = i;				
		}
		else {
			num_cur_tch_temp--;
			dbg("TOUCH id is over Finger: %d, %d/%d\n", touch.abs[CY_TCH_T], num_cur_tch_temp, num_cur_tch);
		}			

// 			printk("[++++ cyttsp4: %d] %d, %d, (%d, %d), %d, %d, %d, %d\n",
// 				num_cur_tch, 
// 				i, 
// 				touch.abs[CY_TCH_T], 
// 				touch.abs[CY_TCH_X],
// 				touch.abs[CY_TCH_Y], 
// 				touch.abs[CY_TCH_P], 
// 				touch.abs[CY_TCH_E], 
// 				touch.abs[CY_TCH_O], 
// 				touch.abs[CY_TCH_W]
// 			);
	}

	num_cur_tch = num_cur_tch_temp;
//-- p11309 

	if ( num_cur_tch == 0 && ts->num_prv_tch != 0 ) {
		
		for (i=0; i < CY_MAX_FINGERNUM; i++)
		{
			if ( finger_info[i].abs[CY_TCH_T] >= 0 ) {
				finger_info[i].abs[CY_TCH_T] = -1;
				input_mt_slot(ts->input, i);				
				input_report_abs(ts->input, ABS_MT_TRACKING_ID, finger_info[i].abs[CY_TCH_T]);

//				printk("[++++ cyttsp4: All Release] %d\n", i);
			}
		}

		input_report_key(ts->input, BTN_TOUCH, 0 );
		input_sync(ts->input);
//		printk("[++++ cyttsp4: Sync]\n");
	}
	else if ( num_cur_tch == 0 && ts->num_prv_tch == 0 ) 
	{
	}
	else {

		for (i=0; i<CY_MAX_FINGERNUM; i++) {

			if ( touch_mask[i] < 0 ) 
			{
				if ( finger_info[i].abs[CY_TCH_T] >= 0 ) {
					finger_info[i].abs[CY_TCH_T] = -1;
					input_mt_slot(ts->input, i);					
					input_report_abs(ts->input, ABS_MT_TRACKING_ID, finger_info[i].abs[CY_TCH_T]);

//					printk("[++++ cyttsp4: Release_miss] %d,%d (%d, %d)\n", i, touch.abs[CY_TCH_T], touch.abs[CY_TCH_X], touch.abs[CY_TCH_Y]);
				}
			}
			else {

				_cyttsp4_get_touch(ts, &touch, ts->xy_data + (touch_mask[i] * ts->si_ofs.tch_rec_siz));

				input_mt_slot(ts->input, i);
				if ( touch.abs[CY_TCH_E] == 3 ) {
					if ( finger_info[i].abs[CY_TCH_T] >= 0 ) {
						finger_info[i].abs[CY_TCH_T] = -1;						
						input_report_abs(ts->input, ABS_MT_TRACKING_ID, finger_info[i].abs[CY_TCH_T]);
						num_cur_tch--;

//						printk("[++++ cyttsp4: Release_event] %d,%d (%d, %d)\n", i, touch.abs[CY_TCH_T], touch.abs[CY_TCH_X], touch.abs[CY_TCH_Y]);
					}
				}
				else 
				{
					if ( finger_info[i].abs[CY_TCH_T] == -1 ) {
						finger_info[i].abs[CY_TCH_T] = input_mt_new_trkid(ts->input);
						input_report_abs(ts->input, ABS_MT_TRACKING_ID, finger_info[i].abs[CY_TCH_T]);						

//						printk("[++++ cyttsp4: Press] %d,%d (%d, %d)\n", i, touch.abs[CY_TCH_T], touch.abs[CY_TCH_X], touch.abs[CY_TCH_Y]);
					}					
					else {
//						printk("[++++ cyttsp4: Move] %d,%d (%d, %d)\n", i, touch.abs[CY_TCH_T], touch.abs[CY_TCH_X], touch.abs[CY_TCH_Y]);
					}

					finger_info[touch.abs[CY_TCH_T]].abs[CY_TCH_X] = touch.abs[CY_TCH_X];
					finger_info[touch.abs[CY_TCH_T]].abs[CY_TCH_Y] = touch.abs[CY_TCH_Y];
					finger_info[touch.abs[CY_TCH_T]].abs[CY_TCH_P] = touch.abs[CY_TCH_P];
					finger_info[touch.abs[CY_TCH_T]].abs[CY_TCH_E] = touch.abs[CY_TCH_E];
					finger_info[touch.abs[CY_TCH_T]].abs[CY_TCH_O] = touch.abs[CY_TCH_O];
					finger_info[touch.abs[CY_TCH_T]].abs[CY_TCH_W] = touch.abs[CY_TCH_W];

					input_report_abs(ts->input, ABS_MT_POSITION_X, touch.abs[CY_TCH_X]);
					input_report_abs(ts->input, ABS_MT_POSITION_Y, touch.abs[CY_TCH_Y]);
					input_report_abs(ts->input, ABS_MT_PRESSURE, touch.abs[CY_TCH_P]);
					input_report_abs(ts->input, ABS_MT_WIDTH_MAJOR, touch.abs[CY_TCH_W]);
#ifdef CY_USE_TMA400_SP2
#ifdef CY_USE_TMA400
					input_report_abs(ts->input, ABS_MT_TOUCH_MAJOR, touch.abs[CY_TCH_MAJ]);
					input_report_abs(ts->input, ABS_MT_TOUCH_MINOR, touch.abs[CY_TCH_MIN]);
					input_report_abs(ts->input, ABS_MT_ORIENTATION, touch.abs[CY_TCH_OR]);
#endif /* --CY_USE_TMA400 */
#endif /* --CY_USE_TMA400_SP2 */
				}
			}			
		}		
		
		input_report_key(ts->input, BTN_TOUCH, !!num_cur_tch);
		input_sync(ts->input);

//		printk("[++++ cyttsp4: Sync] %d\n", !!num_cur_tch);
	}	


_cyttsp4_xy_worker_exit:

	ts->num_prv_tch = num_cur_tch;

#ifdef CY_USE_LEVEL_IRQ
    udelay(500);
#endif
    return retval;
}

#ifdef CY_USE_WATCHDOG
#define CY_TIMEOUT msecs_to_jiffies(1000)
static void _cyttsp4_start_wd_timer(struct cyttsp4 *ts)
{
    mod_timer(&ts->timer, jiffies + CY_TIMEOUT);
    return;
}

static void _cyttsp4_stop_wd_timer(struct cyttsp4 *ts)
{
    del_timer(&ts->timer);
    cancel_work_sync(&ts->work);

    return;
}

static void cyttsp4_timer_watchdog(struct work_struct *work)
{
    struct cyttsp4 *ts = container_of(work, struct cyttsp4, work);
    u8 rep_stat = 0;
    int retval = 0;

    if (ts == NULL) {
        dev_err(ts->dev,"%s: NULL context pointer\n", __func__);
        return;
    }

    mutex_lock(&ts->data_lock);
    if (ts->driver_state == CY_ACTIVE_STATE) {
        retval = _cyttsp4_load_status_regs(ts);
        if (retval < 0) {
            dev_err(ts->dev, "%s: failed to access device in watchdog timer r=%d\n", __func__, retval);
            _cyttsp4_queue_startup(ts, false);
            goto cyttsp4_timer_watchdog_exit_error;
        }
        rep_stat = ts->xy_mode[ts->si_ofs.rep_ofs + 1];
        if (IS_BOOTLOADERMODE(rep_stat)) {
            dev_err(ts->dev,
                    "%s: device found in bootloader mode"
                    " when operational mode rep_stat=0x%02X\n",
                    __func__, rep_stat);
            _cyttsp4_queue_startup(ts, false);
            goto cyttsp4_timer_watchdog_exit_error;
        }
    }

    _cyttsp4_start_wd_timer(ts);
cyttsp4_timer_watchdog_exit_error:
    mutex_unlock(&ts->data_lock);
    return;
}

static void cyttsp4_timer(unsigned long handle)
{
    struct cyttsp4 *ts = (struct cyttsp4 *)handle;

    if (!work_pending(&ts->work))
        schedule_work(&ts->work);

    return;
}
#endif

static int _cyttsp4_soft_reset(struct cyttsp4 *ts)
{
    u8 cmd = CY_SOFT_RESET_MODE;

    return _cyttsp4_write_block_data(ts, CY_REG_BASE,
                                     sizeof(cmd), &cmd,
                                     ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
}

static int _cyttsp4_reset(struct cyttsp4 *ts)
{
    enum cyttsp4_driver_state tmp_state = ts->driver_state;
    int retval = 0;

    if (ts->platform_data->hw_reset == NULL) {
        retval = _cyttsp4_soft_reset(ts);
        ts->soft_reset_asserted = true;
    } else {
        retval = ts->platform_data->hw_reset();
    }

    ts->current_mode = CY_MODE_BOOTLOADER;
    ts->driver_state = CY_BL_STATE;
    if (tmp_state != CY_BL_STATE)
        _cyttsp4_pr_state(ts);

    return retval;
}

static void cyttsp4_ts_work_func(struct work_struct *work)
{
    struct cyttsp4 *ts = container_of(work, struct cyttsp4, cyttsp4_resume_startup_work);
    int retval = 0;

    mutex_lock(&ts->data_lock);

#ifdef CONFIG_TOUCHSCREEN_DEBUG
    if (ts->bus_ops->tsdebug >= CY_DBG_LVL_1)
        dev_info(ts->dev, "%s: work_func\n", __func__);
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */

//++ p11309 - 2012.09.07 for ghost touch
//     input_mt_sync(ts->input);
//     input_sync(ts->input);
//-- p11309

    retval = _cyttsp4_startup(ts);
    if (retval < 0) {
        dev_err(ts->dev,
                "%s: Startup failed with error code %d\n",
                __func__, retval);
        _cyttsp4_change_state(ts, CY_IDLE_STATE);
#ifdef CY_USE_WATCHDOG
	} else {
		_cyttsp4_start_wd_timer(ts);
#endif
    } 
    mutex_unlock(&ts->data_lock);

    return;
}

static int _cyttsp4_enter_sleep(struct cyttsp4 *ts)
{
    int retval = 0;
#if defined(CONFIG_PM_SLEEP) || \
	defined(CONFIG_PM) || \
	defined(CONFIG_HAS_EARLYSUSPEND)
    uint8_t sleep = CY_DEEP_SLEEP_MODE;

    dev_vdbg(ts->dev,"%s: Put the part back to sleep\n", __func__);

    retval = _cyttsp4_write_block_data(ts, CY_REG_BASE,sizeof(sleep), &sleep,ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
    if (retval < 0)
        dev_err(ts->dev,"%s: Failed to write sleep bit r=%d\n",__func__, retval);
    else
        _cyttsp4_change_state(ts, CY_SLEEP_STATE);
#endif
    return retval;
}

static int _cyttsp4_wakeup(struct cyttsp4 *ts)	
{
    int retval = 0;
#if defined(CONFIG_PM_SLEEP) || \
	defined(CONFIG_PM) || \
	defined(CONFIG_HAS_EARLYSUSPEND)
    unsigned long timeout = 0;
    unsigned long uretval = 0;
    u8 hst_mode = 0;
#ifdef CY_USE_TMA400
    u8 rep_stat = 0;
#endif /* --CY_USE_TMA400 */
    int wake = CY_WAKE_DFLT;

    _cyttsp4_change_state(ts, CY_CMD_STATE);
    INIT_COMPLETION(ts->int_running);

    if (ts->platform_data->hw_recov == NULL) {
        dev_vdbg(ts->dev,"%s: no hw_recov function\n", __func__);
        retval = -ENOSYS;
    } else {
        /* wake using strobe on host alert pin */
        retval = ts->platform_data->hw_recov(wake);
        if (retval < 0) {
            if (retval == -ENOSYS) {
                dev_vdbg(ts->dev,"%s: no hw_recov wake code=%d" " function\n", __func__, wake);
            } else {
                dev_err(ts->dev,"%s: fail hw_recov(wake=%d)" " function r=%d\n",__func__, wake, retval);
                retval = -ENOSYS;
            }
        }
    }

    if (retval == -ENOSYS) {
        /*
         * Wake the chip with bus traffic
         * The first few reads should always fail because
         * the part is not ready to respond,
         * but the retries should succeed.
         */
        /*
         * Even though this is hardware-specific, it is done
         * here because the board config file doesn't have
         * access to the bus read routine
         */
        retval = _cyttsp4_read_block_data(ts, CY_REG_BASE, sizeof(hst_mode), &hst_mode, ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
        if (retval < 0) {
            /* device may not be ready even with the
             * bus read retries so just go ahead and
             * wait for the cmd rdy interrupt or timeout
             */
            retval = 0;
        } else {
            /* IC is awake but still need to check for
             * proper mode
             */
        }
    } else
        retval = 0;

    /* Wait for cmd rdy interrupt to signal device wake */
    timeout = msecs_to_jiffies(0);
    mutex_unlock(&ts->data_lock);
    uretval = wait_for_completion_interruptible_timeout(&ts->int_running, timeout);
    mutex_lock(&ts->data_lock);

    /* read registers even if wait ended with timeout */
    retval = _cyttsp4_read_block_data(ts,CY_REG_BASE, sizeof(hst_mode), &hst_mode,ts->platform_data->addr[CY_TCH_ADDR_OFS], true);

    /* TMA884 indicates bootloader mode by changing addr */
    if (retval < 0) {
        dev_err(ts->dev,"%s: failed to resume or in bootloader (r=%d)\n",	__func__, retval);
    }
    else {
#ifdef CY_USE_TMA400
        /* read rep stat register for bootloader status */
        retval = _cyttsp4_load_status_regs(ts);

        if (retval < 0) {
            dev_err(ts->dev,"%s: failed to access device on resume r=%d\n",__func__, retval);
            goto _cyttsp4_wakeup_exit;
        }
        rep_stat = ts->xy_mode[ts->si_ofs.rep_ofs + 1];
        if (IS_BOOTLOADERMODE(rep_stat)) {
            dev_err(ts->dev,"%s: device in bootloader mode on wakeup"	" rep_stat=0x%02X\n",__func__, rep_stat);
            retval = -EIO;
            goto _cyttsp4_wakeup_exit;
        }
#endif /* --CY_USE_TMA400 */
        retval = _cyttsp4_handshake(ts, hst_mode);
        if (retval < 0) {
            dev_err(ts->dev,"%s: fail resume INT handshake (r=%d)\n",__func__, retval);
            /* continue; rely on handshake tmo */
            retval = 0;
        }
        _cyttsp4_change_state(ts, CY_ACTIVE_STATE);
    }
#ifdef CY_USE_TMA400
_cyttsp4_wakeup_exit:
#endif /* --CY_USE_TMA400 */
#endif
    return retval;
}

#if defined(CONFIG_PM) || \
	defined(CONFIG_PM_SLEEP) || \
	defined(CONFIG_HAS_EARLYSUSPEND)

#if defined(CONFIG_HAS_EARLYSUSPEND)
int cyttsp4_suspend(void *handle)
{
    struct cyttsp4 *ts = handle;
#elif defined(CONFIG_PM_SLEEP)
static int cyttsp4_suspend(struct device *dev)
{
	struct cyttsp4 *ts = dev_get_drvdata(dev);
#else
int cyttsp4_suspend(void *handle)
{
	struct cyttsp4 *ts = handle;
#endif

    int retval = 0;
#ifdef CONFIG_TOUCHSCREEN_DEBUG
    if (ts->bus_ops->tsdebug >= CY_DBG_LVL_1)
        dev_info(ts->dev,"%s: start Suspend\n", __func__);
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */

    if (ts->test.cur_mode != CY_TEST_MODE_NORMAL_OP)
    {
        retval = -EBUSY;
        dev_err(ts->dev,"%s: Suspend Blocked while in test mode=%d\n",	__func__, ts->test.cur_mode);
    } 
    else
    {
        switch (ts->driver_state) 
        {
        case CY_ACTIVE_STATE:
#if defined(CY_USE_FORCE_LOAD) || defined(CONFIG_TOUCHSCREEN_DEBUG)
			if (ts->waiting_for_fw) {
				retval = -EBUSY;
				dev_err(ts->dev,
			"%s: Suspend Blocked while waiting for"
					" fw load in %s state\n", __func__,
					cyttsp4_driver_state_string
					[ts->driver_state]);
				break;
			}
#endif /* --CY_USE_FORCE_LOAD || CONFIG_TOUCHSCREEN_DEBUG */

            dev_vdbg(ts->dev,"%s: Suspending...\n", __func__);
#ifdef CY_USE_WATCHDOG
            _cyttsp4_stop_wd_timer(ts);
#endif /* --CY_USE_WATCHDOG */
            if (ts->irq_enabled)
                disable_irq(ts->irq);

#ifdef CY_USE_PROTOCOL_TYPE_B
			_cyttsp4_clear_finger_info(ts);
//++ p11309
			input_mt_destroy_slots(ts->input);
//-- p11309
#endif /* --CY_USE_PROTOCOL_TYPE_B */

            mutex_lock(&ts->data_lock);

            retval = _cyttsp4_enter_sleep(ts);
            if (retval < 0)
                dev_err(ts->dev,"%s: fail enter sleep r=%d\n",	__func__, retval);
            else
                _cyttsp4_change_state(ts, CY_SLEEP_STATE);
            mutex_unlock(&ts->data_lock);
            break;
        case CY_SLEEP_STATE:
            dev_err(ts->dev,"%s: already in Sleep state\n", __func__);
            break;
            /*
             * These states could be changing the device state
             * Some of these states don't directly change device state
             * but the next state could happen at any time and that
             * state DOES modify the device state
             * they must complete before allowing suspend.
             */
        case CY_BL_STATE:
        case CY_CMD_STATE:
        case CY_SYSINFO_STATE:
        case CY_READY_STATE:
        case CY_TRANSFER_STATE:
            retval = -EBUSY;
            dev_err(ts->dev,"%s: Suspend Blocked while in %s state\n",
                    __func__, cyttsp4_driver_state_string[ts->driver_state]);
            break;
        case CY_IDLE_STATE:
        case CY_INVALID_STATE:
        default:
            dev_err(ts->dev,"%s: Cannot enter suspend from %s state\n",
                    __func__, cyttsp4_driver_state_string[ts->driver_state]);
            break;
        }
    }
    
#ifdef CONFIG_TOUCHSCREEN_DEBUG
    if (ts->bus_ops->tsdebug >= CY_DBG_LVL_1)
        dev_info(ts->dev,"%s: exit Suspend r=%d\n", __func__, retval);
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */

    ResumeCnt_T=Init_Resume_T;
    Pow_ON_T=0;
    return retval;
}
EXPORT_SYMBOL_GPL(cyttsp4_suspend);

#if defined(CONFIG_HAS_EARLYSUSPEND)
int cyttsp4_resume(void *handle)
{
    struct cyttsp4 *ts = handle;
#elif defined(CONFIG_PM_SLEEP)
static int cyttsp4_resume(struct device *dev)
{
	struct cyttsp4 *ts = dev_get_drvdata(dev);
#else
int cyttsp4_resume(void *handle)
{
	struct cyttsp4 *ts = handle;
#endif
    int retval = 0;

    mutex_lock(&ts->data_lock);
#ifdef CONFIG_TOUCHSCREEN_DEBUG
    if (ts->bus_ops->tsdebug >= CY_DBG_LVL_1)
        dev_info(ts->dev,"%s: start Resume\n", __func__);
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */

#ifdef CY_USE_LEVEL_IRQ
    /* workaround level interrupt unmasking issue */
    if (ts->irq_enabled) {
        disable_irq_nosync(ts->irq);
        udelay(5);
        enable_irq(ts->irq);
    }
#endif

    switch (ts->driver_state) {
    case CY_SLEEP_STATE:
        retval = _cyttsp4_wakeup(ts);
        if (retval < 0)
        {
            dev_err(ts->dev,"%s: wakeup fail r=%d\n",__func__, retval);
            _cyttsp4_pr_state(ts);
            if (ts->irq_enabled)
                enable_irq(ts->irq);
            _cyttsp4_queue_startup(ts, false);
            break;
        }
        _cyttsp4_change_state(ts, CY_ACTIVE_STATE);

//++ p11309
#ifdef CY_USE_PROTOCOL_TYPE_B
		input_mt_init_slots(ts->input, CY_MAX_FINGERNUM);
#endif 
//-- p11309
        
		if (ts->irq_enabled) 
			enable_irq(ts->irq);

#ifdef CY_USE_CHARGER_MODE
        cyttsp4_chargermode_reset(ts);
#endif /* --CY_USE_CHARGER_MODE */
#ifdef CY_USE_WATCHDOG
        _cyttsp4_start_wd_timer(ts);
#endif
        break;

//++ p11309 2012.09.17 for abnormal state
	case CY_BL_STATE:
    case CY_IDLE_STATE:
		_cyttsp4_reset(ts);
		_cyttsp4_startup(ts);
		break;
//-- p11309
	
    case CY_READY_STATE:
    case CY_ACTIVE_STATE:
    case CY_SYSINFO_STATE:
    case CY_CMD_STATE:
    case CY_TRANSFER_STATE:
    case CY_INVALID_STATE:
    default:
        dev_err(ts->dev,"%s: Already in %s state\n", __func__,	cyttsp4_driver_state_string[ts->driver_state]);
        break;
    }
    mutex_unlock(&ts->data_lock);

    Pow_ON_T=1;
    ResumeCnt_T=Init_Resume_T;

#ifdef CONFIG_TOUCHSCREEN_DEBUG
    if (ts->bus_ops->tsdebug >= CY_DBG_LVL_1)
        dev_info(ts->dev,"%s: exit Resume r=%d\n", __func__, retval);
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */

    return  retval;
}
EXPORT_SYMBOL_GPL(cyttsp4_resume);
#endif

#if !defined(CONFIG_HAS_EARLYSUSPEND) && defined(CONFIG_PM_SLEEP)
const struct dev_pm_ops cyttsp4_pm_ops = {
    SET_SYSTEM_SLEEP_PM_OPS(cyttsp4_suspend, cyttsp4_resume)
};
EXPORT_SYMBOL_GPL(cyttsp4_pm_ops);
#endif


#if defined(CONFIG_HAS_EARLYSUSPEND)
void cyttsp4_early_suspend(struct early_suspend *h)
{
    struct cyttsp4 *ts = container_of(h, struct cyttsp4, early_suspend);
    int retval = 0;

    dev_vdbg(ts->dev, "%s: EARLY SUSPEND ts=%p\n",	__func__, ts);
    retval = cyttsp4_suspend(ts);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Early suspend failed with error code %d\n",__func__, retval);
    }
}
void cyttsp4_late_resume(struct early_suspend *h)
{
    struct cyttsp4 *ts = container_of(h, struct cyttsp4, early_suspend);
    int retval = 0;

    dev_vdbg(ts->dev, "%s: LATE RESUME ts=%p\n",__func__, ts);
    retval = cyttsp4_resume(ts);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Late resume failed with error code %d\n",__func__, retval);
    }
}
#endif

#ifdef CY_AUTO_LOAD_FW
#ifdef CY_CHECK_ADC
static int _cyttsp4_check_adc(int channel)
{
    int rslt;
    void *h;
    struct adc_chan_result acr;
    struct completion conv_completion_evt;
    struct cyttsp4 *ts = container_of(h, struct cyttsp4, early_suspend);

    // open channel
    rslt = adc_channel_open(channel, &h);
    if(rslt) {
        dev_err(ts->dev, "fail to open channel %d. rslt=%d\n",channel, rslt);
        return -EINVAL;
    }

    // start adc convert
    init_completion(&conv_completion_evt);
    rslt = adc_channel_request_conv(h, &conv_completion_evt);
    if(rslt) {
        dev_err(ts->dev, "fail to request channel %d. rslt=%d\n",channel, rslt);
        return -EINVAL;
    }

    // read adc
    wait_for_completion(&conv_completion_evt);
    rslt = adc_channel_read_result(h, &acr);
    if(rslt) {
        dev_err(ts->dev, "fail to read channel %d. rslt=%d\n",channel, rslt);
        return -EINVAL;
    }

    // close channel
    rslt = adc_channel_close(h);
    if(rslt) {
        dev_err(ts->dev, "fail to close channel %d. rslt=%d\n",channel, rslt);
        return -EINVAL;
    }

    // return adc measurement
    return acr.measurement;
}
#endif /* --CY_CHECK_ADC */

static int _cyttsp4_boot_loader(struct cyttsp4 *ts, bool *upgraded)
{
    int retval = 0;
    int i = 0;
    u32 fw_vers_platform = 0;
    u32 fw_vers_img = 0;
    u32 fw_revctrl_platform_h = 0;
    u32 fw_revctrl_platform_l = 0;
    u32 fw_revctrl_img_h = 0;
    u32 fw_revctrl_img_l = 0;
    bool new_fw_vers = false;
    bool new_fw_revctrl = false;
    bool new_vers = false;

#ifdef CY_CHECK_ADC
    int adc_measurement;
    adc_measurement = _cyttsp4_check_adc(0x84);
    dev_info(ts->dev, "%s: adc_measurement=%d\n",__func__,adc_measurement);
#endif /* --CY_CHECK_ADC */

    *upgraded = false;
    if (ts->driver_state == CY_SLEEP_STATE) {
        dev_err(ts->dev,
                "%s: cannot load firmware in sleep state\n",
                __func__);
        retval = 0;
    } else if ((ts->platform_data->fw->ver == NULL) ||
               (ts->platform_data->fw->img == NULL)) {
        dev_err(ts->dev,
                "%s: empty version list or no image\n",
                __func__);
        retval = 0;
    } else if (ts->platform_data->fw->vsize != CY_BL_VERS_SIZE) {
        dev_err(ts->dev,
                "%s: bad fw version list size=%d\n",
                __func__, ts->platform_data->fw->vsize);
        retval = 0;
    } else {
        /* automatically update firmware if new version detected */
        fw_vers_img = (ts->sysinfo_ptr.cydata->fw_ver_major * 256);
        fw_vers_img += ts->sysinfo_ptr.cydata->fw_ver_minor;
        fw_vers_platform = ts->platform_data->fw->ver[2] * 256;
        fw_vers_platform += ts->platform_data->fw->ver[3];
#ifdef CY_ANY_DIFF_NEW_VER
        if (fw_vers_platform != fw_vers_img)
            new_fw_vers = true;
        else
            new_fw_vers = false;
#else
        if (fw_vers_platform > fw_vers_img)
            new_fw_vers = true;
        else
            new_fw_vers = false;
#endif

        dbg("fw_vers_platform=%04X fw_vers_img=%04X\n", fw_vers_platform, fw_vers_img);

        fw_revctrl_img_h = ts->sysinfo_ptr.cydata->revctrl[0];
        fw_revctrl_img_l = ts->sysinfo_ptr.cydata->revctrl[4];
        fw_revctrl_platform_h = ts->platform_data->fw->ver[4];
        fw_revctrl_platform_l = ts->platform_data->fw->ver[8];
        for (i = 1; i < 4; i++) {
            fw_revctrl_img_h = (fw_revctrl_img_h * 256) +
                ts->sysinfo_ptr.cydata->revctrl[0+i];
            fw_revctrl_img_l = (fw_revctrl_img_l * 256) +
                ts->sysinfo_ptr.cydata->revctrl[4+i];
            fw_revctrl_platform_h = (fw_revctrl_platform_h * 256) +
                ts->platform_data->fw->ver[4+i];
            fw_revctrl_platform_l = (fw_revctrl_platform_l * 256) +
                ts->platform_data->fw->ver[8+i];
        }
#ifdef CY_ANY_DIFF_NEW_VER
        if (fw_revctrl_platform_h != fw_revctrl_img_h)
            new_fw_revctrl = true;
        else if (fw_revctrl_platform_h == fw_revctrl_img_h) {
            if (fw_revctrl_platform_l != fw_revctrl_img_l)
                new_fw_revctrl = true;
            else
                new_fw_revctrl = false;
        } else
            new_fw_revctrl = false;
#else
        if (fw_revctrl_platform_h > fw_revctrl_img_h)
            new_fw_revctrl = true;
        else if (fw_revctrl_platform_h == fw_revctrl_img_h) {
            if (fw_revctrl_platform_l > fw_revctrl_img_l)
                new_fw_revctrl = true;
            else
                new_fw_revctrl = false;
        } else
            new_fw_revctrl = false;
#endif

		if (new_fw_vers || new_fw_revctrl)
			new_vers = true;

//++ p11309 -20120908
#ifdef CYTTSP4_PREVENT_REDOWNLOAD
		download_count +=new_vers;
		if ( download_count > 2 ) new_vers = false;
#endif
//-- p11309

		dbg("fw_revctrl_platform_h=%08X, fw_revctrl_img_h=%08X\n", fw_revctrl_platform_h, fw_revctrl_img_h);
		dbg("fw_revctrl_platform_l=%08X, fw_revctrl_img_l=%08X\n", fw_revctrl_platform_l, fw_revctrl_img_l);
		dbg("new_fw_vers=%d, new_fw_revctrl=%d, new_vers=%d\n", (int)new_fw_vers, (int)new_fw_revctrl, (int)new_vers);

        if (new_vers) {
            dev_info(ts->dev, "%s: upgrading firmware...\n", __func__);
            retval = _cyttsp4_load_app(ts,
                                       ts->platform_data->fw->img,
                                       ts->platform_data->fw->size);
            if (retval < 0) {
                dev_err(ts->dev,
                        "%s: communication fail"
                        " on load fw r=%d\n",
                        __func__, retval);
                _cyttsp4_change_state(ts, CY_IDLE_STATE);
                retval = -EIO;
            } else {
                dev_info(ts->dev, "%s: upgraded firmware...\n", __func__);
                *upgraded = true;
            }
        } else {
            dev_vdbg(ts->dev,
                     "%s: No auto firmware upgrade required\n",
                     __func__);
        }
    }

    return retval;
}
#endif /* --CY_AUTO_LOAD_FW */

#ifdef CY_USE_TMA884
static int _cyttsp4_write_mddata(struct cyttsp4 *ts, size_t write_length,
                                 size_t mddata_length, u8 blkid, size_t mddata_ofs,
                                 u8 *ic_buf, const char *mddata_name)
{
    bool mddata_updated = false;
    u8 *pdata;
    int retval = 0;

    pdata = kzalloc(CY_MAX_PRBUF_SIZE, GFP_KERNEL);
    if (pdata == NULL) {
        dev_err(ts->dev,
                "%s: Fail allocate data buffer\n", __func__);
        retval = -ENOMEM;
        goto cyttsp4_write_mddata_exit;
    }
    if (ts->current_mode != CY_MODE_OPERATIONAL) {
        dev_err(ts->dev,
                "%s: Must be in operational mode to start write of"
                " %s (current mode=%d)\n",
                __func__, mddata_name, ts->current_mode);
        retval = -EPERM;
        goto cyttsp4_write_mddata_exit;
    }
    if ((write_length + ts->ic_grpoffset) > mddata_length) {
        dev_err(ts->dev,
                "%s: Requested length(%d) is greater than"
                " %s size(%d)\n", __func__,
                write_length, mddata_name, mddata_length);
        retval = -EINVAL;
        goto cyttsp4_write_mddata_exit;
    }
    retval = _cyttsp4_set_mode(ts, CY_SYSINFO_MODE);
    if (retval < 0) {
        dev_err(ts->dev,
                "%s: Fail to enter Sysinfo mode r=%d\n",
                __func__, retval);
        goto cyttsp4_write_mddata_exit;
    }
    dev_vdbg(ts->dev,
             "%s: blkid=%02X mddata_ofs=%d mddata_length=%d"
             " mddata_name=%s write_length=%d grpofs=%d\n",
             __func__, blkid, mddata_ofs, mddata_length, mddata_name,
             write_length, ts->ic_grpoffset);
    _cyttsp4_read_block_data(ts, mddata_ofs, mddata_length, pdata,
                             ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
    if (retval < 0) {
        dev_err(ts->dev,
                "%s: Fail to read %s regs r=%d\n",
                __func__, mddata_name, retval);
        goto cyttsp4_write_mddata_exit;
    }
    memcpy(pdata + ts->ic_grpoffset, ic_buf, write_length);
    _cyttsp4_set_data_block(ts, blkid, pdata,
                            mddata_length, mddata_name, true, &mddata_updated);
    if ((retval < 0) || !mddata_updated) {
        dev_err(ts->dev,
                "%s: Fail while writing %s block r=%d updated=%d\n",
                __func__, mddata_name, retval, (int)mddata_updated);
    }
    retval = _cyttsp4_set_mode(ts, CY_OPERATE_MODE);
    if (retval < 0) {
        dev_err(ts->dev,
                "%s: Fail to enter Operational mode r=%d\n",
                __func__, retval);
    }

cyttsp4_write_mddata_exit:
    if(pdata != NULL)
        kfree(pdata);
    return retval;
}
#endif /* --CY_USE_TMA884 */

static u16 _cyttsp4_compute_crc(struct cyttsp4 *ts, u8 *buf, int size)
{
    u16 crc = 0xffff;
    u16 tmp;
    int i;

    /* RUN CRC */

    if (size == 0)
        crc = ~crc;
    else {
        do {
            for (i = 0, tmp = 0x00ff & *buf++; i < 8;
                 i++, tmp >>= 1) {
                if ((crc & 0x0001) ^ (tmp & 0x0001))
                    crc = (crc >> 1) ^ 0x8408;
                else
                    crc >>= 1;
            }
        } while (--size);

        crc = ~crc;
        tmp = crc;
        crc = (crc << 8) | (tmp >> 8 & 0xFF);
    }

    return crc;
}

static int _cyttsp4_get_status(struct cyttsp4 *ts,
                               u8 *buf, int size, unsigned long timeout_ms)
{
    unsigned long uretval = 0;
    int tries = 0;
    int retval = 0;

    if (timeout_ms != 0) {
        /* wait until status ready interrupt or timeout occurs */
        uretval = wait_for_completion_interruptible_timeout(&ts->int_running, msecs_to_jiffies(timeout_ms));

        /* read the status packet */
        if (buf == NULL) {
            dev_err(ts->dev,"%s: Status buf ptr is NULL\n", __func__);
            retval = -EINVAL;
            goto _cyttsp4_get_status_exit;
        }
        for (tries = 0; tries < 2; tries++) {
            retval = _cyttsp4_read_block_data(ts, CY_REG_BASE, size,
                                              buf, ts->platform_data->addr[CY_LDR_ADDR_OFS],
#ifdef CY_USE_TMA400
                                              true);
#endif /* --CY_USE_TMA400 */
#ifdef CY_USE_TMA884
            false);
#endif /* --CY_USE_TMA884 */
            /*
             * retry if bus read error or
             * status byte shows not ready
             */
            if ((buf[1] == CY_COMM_BUSY) || (buf[1] == CY_CMD_BUSY))
                msleep(CY_DELAY_DFLT);
            else
                break;
        }
        dev_vdbg(ts->dev,"%s: tries=%d ret=%d status=%02X\n",__func__, tries, retval, buf[1]);
    }

_cyttsp4_get_status_exit:
    mutex_lock(&ts->data_lock);
    return retval;
}

/*
 * Send a bootloader command to the device;
 * Wait for the ISR to execute indicating command
 * was received and status is ready;
 * Releases data_lock mutex to allow ISR to run,
 * then locks it again.
 */
static int _cyttsp4_send_cmd(struct cyttsp4 *ts, const u8 *cmd_buf,
                             int cmd_size, u8 *stat_ret, size_t num_stat_byte,
                             size_t status_size, unsigned long timeout_ms)
{
    u8 *status_buf = NULL;
    int retval = 0;

    if (timeout_ms > 0) {
        status_buf = kzalloc(CY_MAX_STATUS_SIZE, GFP_KERNEL);
        if (status_buf == NULL) {
            dev_err(ts->dev,"%s: Fail alloc status buffer=%p\n",__func__, status_buf);
            goto _cyttsp4_send_cmd_exit;
        }
    }

    if (cmd_buf == NULL) {
        dev_err(ts->dev,"%s: bad cmd_buf=%p\n", __func__, cmd_buf);
        goto _cyttsp4_send_cmd_exit;
    }

    if (cmd_size == 0) {
        dev_err(ts->dev,"%s: bad cmd_size=%d\n", __func__, cmd_size);
        goto _cyttsp4_send_cmd_exit;
    }

    _cyttsp4_pr_buf(ts, (u8 *)cmd_buf, cmd_size, "send_cmd");

    mutex_unlock(&ts->data_lock);
    if (timeout_ms > 0)
        INIT_COMPLETION(ts->int_running);
    retval = _cyttsp4_write_block_data(ts, CY_REG_BASE, cmd_size, cmd_buf, ts->platform_data->addr[CY_LDR_ADDR_OFS],
#ifdef CY_USE_TMA400
                                       true);
#endif /* --CY_USE_TMA400 */
#ifdef CY_USE_TMA884
    false);
#endif /* --CY_USE_TMA884 */
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail writing command=%02X\n",__func__, cmd_buf[CY_CMD_BYTE]);
        mutex_lock(&ts->data_lock);
        goto _cyttsp4_send_cmd_exit;
    }

    /* get the status and lock the mutex */
    if (timeout_ms > 0) {
        retval = _cyttsp4_get_status(ts, status_buf,status_size, timeout_ms);
        if ((retval < 0) || (status_buf[0] != CY_START_OF_PACKET)) {
            dev_err(ts->dev,"%s: Error getting status r=%d status_buf[0]=%02X\n",
                    __func__, retval, status_buf[0]);
            if (!(retval < 0))
                retval = -EIO;
            goto _cyttsp4_send_cmd_exit;
        } else {
            if (status_buf[CY_STATUS_BYTE] != ERROR_SUCCESS) {
                dev_err(ts->dev,"%s: Status=0x%02X error\n",__func__, status_buf[CY_STATUS_BYTE]);
                retval = -EIO;
            } else if (stat_ret != NULL) {
                if (num_stat_byte < status_size)
                    *stat_ret = status_buf[num_stat_byte];
                else
                    *stat_ret = 0;
            }
        }
    } else {
        if (stat_ret != NULL)
            *stat_ret = ERROR_SUCCESS;
        mutex_lock(&ts->data_lock);
    }

_cyttsp4_send_cmd_exit:
    if (status_buf != NULL)
        kfree(status_buf);
    return retval;
}

#if defined(CY_AUTO_LOAD_FW) || \
	defined(CY_USE_FORCE_LOAD) || \
	defined(CONFIG_TOUCHSCREEN_DEBUG)
#define CY_CMD_LDR_ENTER				0x38
#define CY_CMD_LDR_ENTER_CMD_SIZE			7
#define CY_CMD_LDR_ENTER_STAT_SIZE			15
#define CY_CMD_LDR_INIT					0x48
#define CY_CMD_LDR_INIT_CMD_SIZE			15
#define CY_CMD_LDR_INIT_STAT_SIZE			7
#define CY_CMD_LDR_ERASE_ROW				0x34
#define CY_CMD_LDR_ERASE_ROW_CMD_SIZE			10
#define CY_CMD_LDR_ERASE_ROW_STAT_SIZE			7
#define CY_CMD_LDR_SEND_DATA				0x37
#define CY_CMD_LDR_SEND_DATA_CMD_SIZE			4 /* hdr bytes only */
#define CY_CMD_LDR_SEND_DATA_STAT_SIZE			8
#define CY_CMD_LDR_PROG_ROW				0x39
#define CY_CMD_LDR_PROG_ROW_CMD_SIZE			7 /* hdr bytes only */
#define CY_CMD_LDR_PROG_ROW_STAT_SIZE			7
#define CY_CMD_LDR_VERIFY_ROW				0x3A
#define CY_CMD_LDR_VERIFY_ROW_STAT_SIZE			8
#define CY_CMD_LDR_VERIFY_ROW_CMD_SIZE			10
#define CY_CMD_LDR_VERIFY_CHKSUM			0x31
#define CY_CMD_LDR_VERIFY_CHKSUM_CMD_SIZE		7
#define CY_CMD_LDR_VERIFY_CHKSUM_STAT_SIZE		8

#ifdef CONFIG_TOUCHSCREEN_DEBUG
static const char * const ldr_status_string[] = {
	/* Order must match enum ldr_status above */
	"Error Success",
	"Error Command",
	"Error Flash Array",
	"Error Packet Data",
	"Error Packet Length",
	"Error Packet Checksum",
	"Error Flash Protection",
	"Error Flash Checksum",
	"Error Verify Image",
	"Error Invalid Command",
	"Error Invalid Command",
	"Error Invalid Command",
	"Error Invalid Command",
	"Error Invalid Command",
	"Error Invalid Command",
	"Error Invalid Command",
	"Invalid Error Code"
};

static void _cyttsp4_pr_status(struct cyttsp4 *ts, int status)
{
	if (status > ERROR_INVALID)
		status = ERROR_INVALID;
	dev_vdbg(ts->dev,
		"%s: status error(%d)=%s\n",
		__func__, status, ldr_status_string[status]);
}
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */

static u16 _cyttsp4_get_short(u8 *buf)
{
    return ((u16)(*buf) << 8) + *(buf+1);
}

static u8 *_cyttsp4_get_row(struct cyttsp4 *ts,
                            u8 *row_buf, u8 *image_buf, int size)
{
    int i;
    for (i = 0; i < size; i++) {
        /* copy a row from the image */
        row_buf[i] = image_buf[i];
    }

    image_buf = image_buf + size;
    return image_buf;
}

static int _cyttsp4_ldr_enter(struct cyttsp4 *ts, struct cyttsp4_dev_id *dev_id)
{
    u16 crc;
    int i = 0;
    size_t cmd_size;
    u8 status_buf[CY_MAX_STATUS_SIZE];
    u8 status = 0;
    int retval = 0;
    /* +1 for TMA400 host sync byte */
    u8 ldr_enter_cmd[CY_CMD_LDR_ENTER_CMD_SIZE+1];

    memset(status_buf, 0, sizeof(status_buf));
    dev_id->bl_ver = 0;
    dev_id->rev_id = 0;
    dev_id->silicon_id = 0;

#ifdef CY_USE_TMA400
    ldr_enter_cmd[i++] = CY_CMD_LDR_HOST_SYNC;
#endif /* --CY_USE_TMA400 */
    ldr_enter_cmd[i++] = CY_START_OF_PACKET;
    ldr_enter_cmd[i++] = CY_CMD_LDR_ENTER;
    ldr_enter_cmd[i++] = 0x00;	/* data len lsb */
    ldr_enter_cmd[i++] = 0x00;	/* data len msb */
#ifdef CY_USE_TMA400
    crc = _cyttsp4_compute_crc(ts, &ldr_enter_cmd[1], i - 1);
    cmd_size = sizeof(ldr_enter_cmd);
#endif /* --CY_USE_TMA400 */
#ifdef CY_USE_TMA884
    crc = _cyttsp4_compute_crc(ts, ldr_enter_cmd, i);
    cmd_size = sizeof(ldr_enter_cmd) - 1;
#endif /* --CY_USE_TMA884 */
    ldr_enter_cmd[i++] = (u8)crc;
    ldr_enter_cmd[i++] = (u8)(crc >> 8);
    ldr_enter_cmd[i++] = CY_END_OF_PACKET;

    mutex_unlock(&ts->data_lock);
    INIT_COMPLETION(ts->int_running);
    retval = _cyttsp4_write_block_data(ts, CY_REG_BASE, cmd_size,
                                       ldr_enter_cmd, ts->platform_data->addr[CY_LDR_ADDR_OFS],
#ifdef CY_USE_TMA400
                                       true);
#endif /* --CY_USE_TMA400 */
#ifdef CY_USE_TMA884
    false);
#endif /* --CY_USE_TMA884 */
    if (retval < 0) {
        dev_err(ts->dev,"%s: write block failed %d\n", __func__, retval);
        goto _cyttsp4_ldr_enter_exit;
    }

    /* Wait for ISR, get status and lock mutex */
    retval = _cyttsp4_get_status(ts, status_buf,
                                 CY_CMD_LDR_ENTER_STAT_SIZE, CY_HALF_SEC_TMO_MS);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail get status to Enter Loader command r=%d\n",__func__, retval);
    } else {
        status = status_buf[CY_STATUS_BYTE];
        if (status == ERROR_SUCCESS) {
            dev_id->bl_ver = status_buf[11] << 16 |	status_buf[10] <<  8 |status_buf[9] <<  0;
            dev_id->rev_id = status_buf[8] <<  0;
            dev_id->silicon_id = status_buf[7] << 24 |status_buf[6] << 16 |	status_buf[5] <<  8 |status_buf[4] <<  0;
            retval = 0;
        } else
            retval = -EIO;
#ifdef CONFIG_TOUCHSCREEN_DEBUG
        _cyttsp4_pr_status(ts, status);
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */
        dev_vdbg(ts->dev,"%s: status=%d "	"bl_ver=%08X rev_id=%02X silicon_id=%08X\n",
                 __func__, status, dev_id->bl_ver, dev_id->rev_id, dev_id->silicon_id);
    }

_cyttsp4_ldr_enter_exit:
    return retval;
}

#ifdef CY_USE_TMA400
static int _cyttsp4_ldr_init(struct cyttsp4 *ts)
{
    u16 crc;
    int i = 0;
    int retval = 0;
    /* +1 for TMA400 host sync byte */
    u8 ldr_init_cmd[CY_CMD_LDR_INIT_CMD_SIZE+1];

    ldr_init_cmd[i++] = CY_CMD_LDR_HOST_SYNC;
    ldr_init_cmd[i++] = CY_START_OF_PACKET;
    ldr_init_cmd[i++] = CY_CMD_LDR_INIT;
    ldr_init_cmd[i++] = 0x08;	/* data len lsb */
    ldr_init_cmd[i++] = 0x00;	/* data len msb */
    memcpy(&ldr_init_cmd[i], cyttsp4_security_key,
           sizeof(cyttsp4_security_key));
    i += sizeof(cyttsp4_security_key);
    crc = _cyttsp4_compute_crc(ts, &ldr_init_cmd[1], i - 1);
    ldr_init_cmd[i++] = (u8)crc;
    ldr_init_cmd[i++] = (u8)(crc >> 8);
    ldr_init_cmd[i++] = CY_END_OF_PACKET;

    retval = _cyttsp4_send_cmd(ts, ldr_init_cmd, i, NULL, 0,
                               CY_CMD_LDR_INIT_STAT_SIZE, CY_TEN_SEC_TMO_MS);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail ldr init r=%d\n",__func__, retval);
    }

    return retval;
}
#endif /* --CY_USE_TMA400 */

struct cyttsp4_hex_image {
    u8 array_id;
    u16 row_num;
    u16 row_size;
    u8 row_data[CY_DATA_ROW_SIZE];
} __packed;

#ifdef CY_USE_TMA884
static int _cyttsp4_ldr_erase_row(struct cyttsp4 *ts,
                                  struct cyttsp4_hex_image *row_image)
{
    u16 crc;
    int i = 0;
    int retval = 0;
    /* +1 for TMA400 host sync byte */
    u8 ldr_erase_row_cmd[CY_CMD_LDR_ERASE_ROW_CMD_SIZE+1];

#ifdef CY_USE_TMA400
    ldr_erase_row_cmd[i++] = CY_CMD_LDR_HOST_SYNC;
#endif /* --CY_USE_TMA400 */
    ldr_erase_row_cmd[i++] = CY_START_OF_PACKET;
    ldr_erase_row_cmd[i++] = CY_CMD_LDR_ERASE_ROW;
    ldr_erase_row_cmd[i++] = 0x03;	/* data len lsb */
    ldr_erase_row_cmd[i++] = 0x00;	/* data len msb */
    ldr_erase_row_cmd[i++] = row_image->array_id;
    ldr_erase_row_cmd[i++] = (u8)row_image->row_num;
    ldr_erase_row_cmd[i++] = (u8)(row_image->row_num >> 8);
#ifdef CY_USE_TMA400
    crc = _cyttsp4_compute_crc(ts, &ldr_erase_row_cmd[1], i - 1);
#endif /* --CY_USE_TMA400 */
#ifdef CY_USE_TMA884
    crc = _cyttsp4_compute_crc(ts, ldr_erase_row_cmd, i);
#endif /* --CY_USE_TMA884 */
    ldr_erase_row_cmd[i++] = (u8)crc;
    ldr_erase_row_cmd[i++] = (u8)(crc >> 8);
    ldr_erase_row_cmd[i++] = CY_END_OF_PACKET;

    retval = _cyttsp4_send_cmd(ts, ldr_erase_row_cmd, i, NULL, 0,
                               CY_CMD_LDR_ERASE_ROW_STAT_SIZE, CY_HALF_SEC_TMO_MS);

    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail erase row=%d r=%d\n",__func__, row_image->row_num, retval);
    }
    return retval;
}
#endif

static int _cyttsp4_ldr_parse_row(struct cyttsp4 *ts, u8 *row_buf,
                                  struct cyttsp4_hex_image *row_image)
{
    u16 i, j;
    int retval = 0;

    if (!row_buf) {
        dev_err(ts->dev,"%s parse row error - buf is null\n", __func__);
        retval = -EINVAL;
        goto cyttsp4_ldr_parse_row_exit;
    }

    row_image->array_id = row_buf[CY_ARRAY_ID_OFFSET];
    row_image->row_num = _cyttsp4_get_short(&row_buf[CY_ROW_NUM_OFFSET]);
    row_image->row_size = _cyttsp4_get_short(&row_buf[CY_ROW_SIZE_OFFSET]);

    if (row_image->row_size > ARRAY_SIZE(row_image->row_data)) {
        dev_err(ts->dev,"%s: row data buffer overflow\n", __func__);
        retval = -EOVERFLOW;
        goto cyttsp4_ldr_parse_row_exit;
    }

    for (i = 0, j = CY_ROW_DATA_OFFSET;
         i < row_image->row_size; i++)
        row_image->row_data[i] = row_buf[j++];

    retval = 0;

cyttsp4_ldr_parse_row_exit:
    return retval;
}

static int _cyttsp4_ldr_prog_row(struct cyttsp4 *ts,
                                 struct cyttsp4_hex_image *row_image)
{
    u16 crc;
    int next;
    int data;
    int row_data;
    u16 row_sum = 0;
    size_t data_len;
#ifdef CY_USE_TMA884
    int segment;
#endif /* --CY_USE_TMA884 */
    int retval = 0;

    u8 *cmd = kzalloc(CY_MAX_PACKET_LEN, GFP_KERNEL);

    if (cmd != NULL) {
        row_data = 0;
        row_sum = 0;

#ifdef CY_USE_TMA884
        for (segment = 0; segment <
             (CY_DATA_ROW_SIZE/CY_PACKET_DATA_LEN)-1;
             segment++) {
            next = 0;
            cmd[next++] = CY_START_OF_PACKET;
            cmd[next++] = CY_CMD_LDR_SEND_DATA;
            cmd[next++] = (u8)CY_PACKET_DATA_LEN;
            cmd[next++] = (u8)(CY_PACKET_DATA_LEN >> 8);

            for (data = 0;
                 data < CY_PACKET_DATA_LEN; data++) {
                cmd[next] = row_image->row_data
                    [row_data++];
                row_sum += cmd[next];
                next++;
            }

            crc = _cyttsp4_compute_crc(ts, cmd, next);
            cmd[next++] = (u8)crc;
            cmd[next++] = (u8)(crc >> 8);
            cmd[next++] = CY_END_OF_PACKET;

            retval = _cyttsp4_send_cmd(ts, cmd, next, NULL,
                                       0, CY_CMD_LDR_SEND_DATA_STAT_SIZE,
                                       CY_HALF_SEC_TMO_MS);

            if (retval < 0) {
                dev_err(ts->dev,"%s: send row=%d segment=%d fail r=%d\n",
                        __func__, row_image->row_num,segment, retval);
                goto cyttsp4_ldr_prog_row_exit;
            }
        }
#endif /* --CY_USE_TMA884 */

        next = 0;
#ifdef CY_USE_TMA400
        cmd[next++] = CY_CMD_LDR_HOST_SYNC;
#endif /* --CY_USE_TMA400 */
        cmd[next++] = CY_START_OF_PACKET;
        cmd[next++] = CY_CMD_LDR_PROG_ROW;
        /*
         * include array id size and row id size in CY_PACKET_DATA_LEN
         */
#ifdef CY_USE_TMA400
        data_len = CY_DATA_ROW_SIZE_TMA400;
#endif /* --CY_USE_TMA400 */
#ifdef CY_USE_TMA884
        data_len = CY_PACKET_DATA_LEN;
#endif /* --CY_USE_TMA884 */
        cmd[next++] = (u8)(data_len+3);
        cmd[next++] = (u8)((data_len+3) >> 8);
        cmd[next++] = row_image->array_id;
        cmd[next++] = (u8)row_image->row_num;
        cmd[next++] = (u8)(row_image->row_num >> 8);

        for (data = 0;
             data < data_len; data++) {
            cmd[next] = row_image->row_data[row_data++];
            row_sum += cmd[next];
            next++;
        }

#ifdef CY_USE_TMA400
        crc = _cyttsp4_compute_crc(ts, &cmd[1], next - 1);
#endif /* --CY_USE_TMA400 */
#ifdef CY_USE_TMA884
        crc = _cyttsp4_compute_crc(ts, cmd, next);
#endif /* --CY_USE_TMA884 */
        cmd[next++] = (u8)crc;
        cmd[next++] = (u8)(crc >> 8);
        cmd[next++] = CY_END_OF_PACKET;

        retval = _cyttsp4_send_cmd(ts, cmd, next, NULL, 0,
                                   CY_CMD_LDR_PROG_ROW_STAT_SIZE, CY_HALF_SEC_TMO_MS);

        if (retval < 0) {
            dev_err(ts->dev,"%s: prog row=%d fail r=%d\n",__func__, row_image->row_num, retval);
            goto cyttsp4_ldr_prog_row_exit;
        }

    } else {
        dev_err(ts->dev,"%s prog row error - cmd buf is NULL\n", __func__);
        retval = -EIO;
    }

cyttsp4_ldr_prog_row_exit:
    if (cmd != NULL)
        kfree(cmd);
    return retval;
}

static int _cyttsp4_ldr_verify_row(struct cyttsp4 *ts,
                                   struct cyttsp4_hex_image *row_image)
{
    u16 crc;
    int i = 0;
    u8 verify_checksum;
    int retval = 0;
    /* +1 for TMA400 host sync byte */
    u8 ldr_verify_row_cmd[CY_CMD_LDR_VERIFY_ROW_CMD_SIZE+1];

#ifdef CY_USE_TMA400
    ldr_verify_row_cmd[i++] = CY_CMD_LDR_HOST_SYNC;
#endif /* --CY_USE_TMA400 */
    ldr_verify_row_cmd[i++] = CY_START_OF_PACKET;
    ldr_verify_row_cmd[i++] = CY_CMD_LDR_VERIFY_ROW;
    ldr_verify_row_cmd[i++] = 0x03;	/* data len lsb */
    ldr_verify_row_cmd[i++] = 0x00;	/* data len msb */
    ldr_verify_row_cmd[i++] = row_image->array_id;
    ldr_verify_row_cmd[i++] = (u8)row_image->row_num;
    ldr_verify_row_cmd[i++] = (u8)(row_image->row_num >> 8);
#ifdef CY_USE_TMA400
    crc = _cyttsp4_compute_crc(ts, &ldr_verify_row_cmd[1], i - 1);
#endif /* --CY_USE_TMA400 */
#ifdef CY_USE_TMA884
    crc = _cyttsp4_compute_crc(ts, ldr_verify_row_cmd, i);
#endif /* --CY_USE_TMA884 */
    ldr_verify_row_cmd[i++] = (u8)crc;
    ldr_verify_row_cmd[i++] = (u8)(crc >> 8);
    ldr_verify_row_cmd[i++] = CY_END_OF_PACKET;

    retval = _cyttsp4_send_cmd(ts, ldr_verify_row_cmd, i,
                               &verify_checksum, 4,
                               CY_CMD_LDR_VERIFY_ROW_STAT_SIZE, CY_HALF_SEC_TMO_MS);

    if (retval < 0) {
        dev_err(ts->dev,"%s: verify row=%d fail r=%d\n",__func__, row_image->row_num, retval);
    }

    return retval;
}

static int _cyttsp4_ldr_verify_chksum(struct cyttsp4 *ts, u8 *app_chksum)
{
    u16 crc;
    int i = 0;
    int retval = 0;
    /* +1 for TMA400 host sync byte */
    u8 ldr_verify_chksum_cmd[CY_CMD_LDR_VERIFY_CHKSUM_CMD_SIZE+1];

#ifdef CY_USE_TMA400
    ldr_verify_chksum_cmd[i++] = CY_CMD_LDR_HOST_SYNC;
#endif /* --CY_USE_TMA400 */
    ldr_verify_chksum_cmd[i++] = CY_START_OF_PACKET;
    ldr_verify_chksum_cmd[i++] = CY_CMD_LDR_VERIFY_CHKSUM;
    ldr_verify_chksum_cmd[i++] = 0x00;	/* data len lsb */
    ldr_verify_chksum_cmd[i++] = 0x00;	/* data len msb */
#ifdef CY_USE_TMA400
    crc = _cyttsp4_compute_crc(ts, &ldr_verify_chksum_cmd[1], i - 1);
#endif /* --CY_USE_TMA400 */
#ifdef CY_USE_TMA884
    crc = _cyttsp4_compute_crc(ts, ldr_verify_chksum_cmd, i);
#endif /* --CY_USE_TMA884 */
    ldr_verify_chksum_cmd[i++] = (u8)crc;
    ldr_verify_chksum_cmd[i++] = (u8)(crc >> 8);
    ldr_verify_chksum_cmd[i++] = CY_END_OF_PACKET;

    retval = _cyttsp4_send_cmd(ts, ldr_verify_chksum_cmd, i,
                               app_chksum, 4,
                               CY_CMD_LDR_VERIFY_CHKSUM_STAT_SIZE, CY_HALF_SEC_TMO_MS);

    if (retval < 0) {
        dev_err(ts->dev,
                "%s: verify checksum fail r=%d\n",
                __func__, retval);
    }

    return retval;
}

static int _cyttsp4_load_app(struct cyttsp4 *ts, const u8 *fw, int fw_size)
{
    u8 *p;
#ifdef CY_USE_TMA884
    u8 tries;
#endif
    int ret;
    int retval;	/* need separate return value at exit stage */
    struct cyttsp4_dev_id *file_id = NULL;
    struct cyttsp4_dev_id *dev_id = NULL;
    struct cyttsp4_hex_image *row_image = NULL;
    u8 app_chksum;

    u8 *row_buf = NULL;
    size_t image_rec_size;
    size_t row_buf_size = 1024 > CY_MAX_PRBUF_SIZE ?
        1024 : CY_MAX_PRBUF_SIZE;
    int row_count = 0;

#ifdef CY_USE_TMA400
    image_rec_size = CY_DATA_ROW_SIZE_TMA400 +(sizeof(struct cyttsp4_hex_image) - CY_DATA_ROW_SIZE);
#endif /* --CY_USE_TMA400 */
#ifdef CY_USE_TMA884
    image_rec_size = sizeof(struct cyttsp4_hex_image);
#endif /* --CY_USE_TMA884 */

    if (!fw_size || (fw_size % image_rec_size != 0)) {
        dev_err(ts->dev,"%s: Firmware image is misaligned\n", __func__);
        retval = -EINVAL;
        goto _cyttsp4_load_app_exit;
    }

#ifdef CY_USE_WATCHDOG
    _cyttsp4_stop_wd_timer(ts);
#endif

    dev_info(ts->dev,"%s: start load app\n", __func__);

    row_buf = kzalloc(row_buf_size, GFP_KERNEL);
    row_image = kzalloc(sizeof(struct cyttsp4_hex_image), GFP_KERNEL);
    file_id = kzalloc(sizeof(struct cyttsp4_dev_id), GFP_KERNEL);
    dev_id = kzalloc(sizeof(struct cyttsp4_dev_id), GFP_KERNEL);
    if ((row_buf == NULL) || (row_image == NULL) ||
        (file_id == NULL) || (dev_id == NULL)) {
        dev_err(ts->dev,"%s: Unable to alloc row buffers(%p %p %p %p)\n",
                __func__, row_buf, row_image, file_id, dev_id);
        retval = -ENOMEM;
        goto _cyttsp4_load_app_error_exit;
    }

    p = (u8 *)fw;
    /* Enter Loader and return Silicon ID and Rev */

    retval = _cyttsp4_reset(ts);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail reset device r=%d\n", __func__, retval);
        goto _cyttsp4_load_app_exit;
    }
    retval = _cyttsp4_wait_int(ts, CY_TEN_SEC_TMO_MS);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail waiting for bootloader interrupt\n",__func__);
        goto _cyttsp4_load_app_exit;
    }

    _cyttsp4_change_state(ts, CY_BL_STATE);
    dev_info(ts->dev,	"%s: Send BL Loader Enter\n", __func__);
    retval = _cyttsp4_ldr_enter(ts, dev_id);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Error cannot start Loader (ret=%d)\n",__func__, retval);
        goto _cyttsp4_load_app_error_exit;
    }

    dev_vdbg(ts->dev,"%s: dev: silicon id=%08X rev=%02X bl=%08X\n",
             __func__, dev_id->silicon_id,dev_id->rev_id, dev_id->bl_ver);

#ifdef CY_USE_TMA400
    udelay(1000);
    retval = _cyttsp4_ldr_init(ts);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Error cannot init Loader (ret=%d)\n",__func__, retval);
        goto _cyttsp4_load_app_error_exit;
    }
#endif /* --CY_USE_TMA400 */

    dev_info(ts->dev,"%s: Send BL Loader Blocks\n", __func__);

    while (p < (fw + fw_size)) {
        /* Get row */
        dev_dbg(ts->dev,"%s: read row=%d\n", __func__, ++row_count);
        memset(row_buf, 0, row_buf_size);
        p = _cyttsp4_get_row(ts, row_buf, p, image_rec_size);

        /* Parse row */
        dev_vdbg(ts->dev,"%s: p=%p buf=%p buf[0]=%02X\n", __func__,p, row_buf, row_buf[0]);
        retval = _cyttsp4_ldr_parse_row(ts, row_buf, row_image);
        dev_vdbg(ts->dev,"%s: array_id=%02X row_num=%04X(%d) row_size=%04X(%d)\n", 
                 __func__,row_image->array_id,	row_image->row_num, row_image->row_num,
                 row_image->row_size, row_image->row_size);

        if (retval < 0) {
            dev_err(ts->dev,"%s: Parse Row Error (a=%d r=%d ret=%d\n",
                    __func__, row_image->array_id,	row_image->row_num,	retval);
            goto bl_exit;
        } else {
            dev_vdbg(ts->dev,"%s: Parse Row (a=%d r=%d ret=%d\n",
                     __func__, row_image->array_id,	row_image->row_num, retval);
        }

#ifdef CY_USE_TMA884
        /* erase row */
        tries = 0;
        do {
            retval = _cyttsp4_ldr_erase_row(ts, row_image);
            if (retval < 0) {
                dev_err(ts->dev,"%s: Erase Row Error (array=%d row=%d ret=%d try=%d)\n",
                        __func__, row_image->array_id,	row_image->row_num, retval, tries);
            }
        } while (retval && tries++ < 5);

        if (retval < 0)
            goto _cyttsp4_load_app_error_exit;
#endif

        /* program row */
        retval = _cyttsp4_ldr_prog_row(ts, row_image);
        if (retval < 0) {
            dev_err(ts->dev,"%s: Program Row Error (array=%d row=%d ret=%d)\n",
                    __func__, row_image->array_id,	row_image->row_num, retval);
            goto _cyttsp4_load_app_error_exit;
        }

        /* verify row */
        retval = _cyttsp4_ldr_verify_row(ts, row_image);
        if (retval < 0) {
            dev_err(ts->dev,"%s: Verify Row Error (array=%d row=%d ret=%d)\n",
                    __func__, row_image->array_id,	row_image->row_num, retval);
            goto _cyttsp4_load_app_error_exit;
        }

        dev_vdbg(ts->dev,"%s: array=%d row_cnt=%d row_num=%04X\n",
                 __func__, row_image->array_id, row_count,row_image->row_num);
    }

    /* verify app checksum */
    retval = _cyttsp4_ldr_verify_chksum(ts, &app_chksum);

    dev_dbg(ts->dev,"%s: Application Checksum = %02X r=%d\n",__func__, app_chksum, retval);

    if (retval < 0) {
        dev_err(ts->dev,"%s: ldr_verify_chksum fail r=%d\n", __func__, retval);
        retval = 0;
    }

    /* exit loader */
bl_exit:
    dev_info(ts->dev,	"%s: Send BL Loader Terminate\n", __func__);

    ret = _cyttsp4_ldr_exit(ts);
    if (ret) {
        dev_err(ts->dev,	"%s: Error on exit Loader (ret=%d)\n",__func__, ret);
        retval = ret;
        goto _cyttsp4_load_app_error_exit;
    }

    /*
     * this is a temporary parking state;
     * the driver will always run startup
     * after the loader has completed
     */
    _cyttsp4_change_state(ts, CY_TRANSFER_STATE);
    goto _cyttsp4_load_app_exit;

_cyttsp4_load_app_error_exit:
    _cyttsp4_change_state(ts, CY_BL_STATE);
_cyttsp4_load_app_exit:
    if(row_buf != NULL)
        kfree(row_buf);
    if(row_image != NULL)
        kfree(row_image);
    if(file_id != NULL)
        kfree(file_id);
    if(dev_id != NULL)
        kfree(dev_id);
    return retval;
}
#endif /* CY_AUTO_LOAD_FW || CY_USE_FORCE_LOAD || CONFIG_TOUCHSCREEN_DEBUG */

/* Constructs loader exit command and sends via _cyttsp4_send_cmd() */
static int _cyttsp4_ldr_exit(struct cyttsp4 *ts)
{
    u16 crc;
    int i = 0;
    int retval = 0;
    /* +1 for TMA400 host sync byte */
    u8 ldr_exit_cmd[CY_CMD_LDR_EXIT_CMD_SIZE+1];

#ifdef CY_USE_TMA400
    ldr_exit_cmd[i++] = CY_CMD_LDR_HOST_SYNC;
#endif /* --CY_USE_TMA400 */
    ldr_exit_cmd[i++] = CY_START_OF_PACKET;
    ldr_exit_cmd[i++] = CY_CMD_LDR_EXIT;
    ldr_exit_cmd[i++] = 0x00;	/* data len lsb */
    ldr_exit_cmd[i++] = 0x00;	/* data len msb */
#ifdef CY_USE_TMA400
    crc = _cyttsp4_compute_crc(ts, &ldr_exit_cmd[1], i - 1);
#endif /* --CY_USE_TMA400 */
#ifdef CY_USE_TMA884
    crc = _cyttsp4_compute_crc(ts, ldr_exit_cmd, i);
#endif /* --CY_USE_TMA884 */
    ldr_exit_cmd[i++] = (u8)crc;
    ldr_exit_cmd[i++] = (u8)(crc >> 8);
    ldr_exit_cmd[i++] = CY_END_OF_PACKET;

    retval = _cyttsp4_send_cmd(ts, ldr_exit_cmd, i, NULL, 0,CY_CMD_LDR_EXIT_STAT_SIZE, 0);

    if (retval < 0) {
        dev_err(ts->dev,"%s: BL Loader exit fail r=%d\n",__func__, retval);
    }

    dev_vdbg(ts->dev,"%s: Exit BL Loader r=%d\n", __func__, retval);

    return retval;
}

#ifdef CY_USE_TMA884
static int _cyttsp4_calc_data_crc(struct cyttsp4 *ts, size_t ndata, u8 *pdata,
                                  u8 *crc_h, u8 *crc_l, const char *name)
{
    int retval = 0;
    u8 *buf = NULL;

    *crc_h = 0;
    *crc_l = 0;

    buf = kzalloc(sizeof(uint8_t) * 126, GFP_KERNEL);
    if (buf == NULL) {
        dev_err(ts->dev,"%s: Failed to allocate buf\n", __func__);
        retval = -ENOMEM;
        goto _cyttsp4_calc_data_crc_exit;
    }

    if (pdata == NULL) {
        dev_err(ts->dev,"%s: bad data pointer\n", __func__);
        retval = -ENXIO;
        goto _cyttsp4_calc_data_crc_exit;
    }

    if (ndata > 122) {
        dev_err(ts->dev,"%s: %s is too large n=%d size=%d\n",__func__, name, ndata, 126);
        retval = -EOVERFLOW;
        goto _cyttsp4_calc_data_crc_exit;
    }

    buf[0] = 0x00; /* num of config bytes + 4 high */
    buf[1] = 0x7E; /* num of config bytes + 4 low */
    buf[2] = 0x00; /* max block size w/o crc high */
    buf[3] = 0x7E; /* max block size w/o crc low */

    /* Copy platform data */
    memcpy(&(buf[4]), pdata, ndata);

    /* Calculate CRC */
    _cyttsp4_calc_crc(ts, buf, 126, crc_h, crc_l);

    dev_vdbg(ts->dev,"%s: crc=%02X%02X\n", __func__, *crc_h, *crc_l);

_cyttsp4_calc_data_crc_exit:
    if(buf != NULL)
        kfree(buf);
    return retval;
}
#endif /* --CY_USE_TMA884 */

#ifdef CONFIG_TOUCHSCREEN_DEBUG
#ifdef CY_USE_TMA400
static int _cyttsp4_calc_ic_crc_tma400(struct cyttsp4 *ts,
	enum cyttsp4_ic_ebid ebid, u8 *crc_h, u8 *crc_l, bool read_back_verify)
{
	u16 crc = 0x0000;
	size_t crc_loc = 0;
	size_t crc_row = 0;
	size_t crc_ofs = 0;
	size_t ndata = 0;
	int row_id = 0;
	u8 *pdata = NULL;
	size_t ntable = 0;
	size_t tsize = 0;
	u8 *ptable = NULL;
	bool match = true;
	int i = 0;
	int retval = 0;

	pdata = kzalloc(ts->ebid_row_size, GFP_KERNEL);
	if (pdata == NULL) {
		dev_err(ts->dev,
			"%s: Fail allocate block buffer\n", __func__);
		retval = -ENOMEM;
		goto _cyttsp4_calc_ic_tch_crc_tma400_exit;
	}

	if (read_back_verify) {
		if (ts->platform_data->sett
			[CY_IC_GRPNUM_TCH_PARM_VAL] == NULL) {
			dev_err(ts->dev,
			"%s: NULL param values table\n",
				__func__);
			goto _cyttsp4_calc_ic_tch_crc_tma400_exit;
		} else if (ts->platform_data->sett
			[CY_IC_GRPNUM_TCH_PARM_VAL]->data == NULL) {
			dev_err(ts->dev,
			"%s: NULL param values table data\n",
				__func__);
			goto _cyttsp4_calc_ic_tch_crc_tma400_exit;
		} else if (ts->platform_data->sett
			[CY_IC_GRPNUM_TCH_PARM_VAL]->size == 0) {
			dev_err(ts->dev,
			"%s: param values table size is 0\n",
				__func__);
			goto _cyttsp4_calc_ic_tch_crc_tma400_exit;
		} else {
			ptable = (u8 *)ts->platform_data->sett
				[CY_IC_GRPNUM_TCH_PARM_VAL]->data;
			tsize = ts->platform_data->sett
				[CY_IC_GRPNUM_TCH_PARM_VAL]->size;
		}
	}

	crc = 0xFFFF;
	row_id = 0;
	dev_vdbg(ts->dev,
		"%s: tch ebid=%d row=%d data:\n", __func__, ebid, row_id);
	retval = _cyttsp4_get_ebid_data_tma400(ts, ebid, row_id, pdata);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: Fail get ebid=%d row=%d data r=%d\n",
			__func__, ebid, row_id, retval);
		retval = -EIO;
		goto _cyttsp4_calc_ic_tch_crc_tma400_exit;
	}
	_cyttsp4_pr_buf(ts, pdata, ts->ebid_row_size, "ebid_data");
	/* determine CRC location */
	crc_loc = (pdata[3] * 256) + pdata[2];
	crc_ofs = crc_loc % ts->ebid_row_size;
	crc_row = crc_loc / ts->ebid_row_size;
	dev_vdbg(ts->dev,
		"%s: tch ebid=%d crc_loc=%08X crc_row=%d crc_ofs=%d data:\n",
		__func__, ebid, crc_loc, crc_row, crc_ofs);

	ndata = 0;
	/* if CRC is in row 0, then the loop is skipped */
	for (row_id = 0; row_id < crc_row; row_id++) {
		dev_vdbg(ts->dev,
			"%s: Get CRC bytes for ebid=%d row=%d crc_row=%d\n",
			__func__, ebid, row_id, crc_row);
		retval = _cyttsp4_get_ebid_data_tma400(ts, ebid, row_id, pdata);
		if (retval < 0) {
			dev_err(ts->dev,
			"%s: Fail get row=%d data r=%d\n",
				__func__, row_id, retval);
			retval = -EIO;
			goto _cyttsp4_calc_ic_tch_crc_tma400_exit;
		}
		_cyttsp4_pr_buf(ts, pdata, ts->ebid_row_size, "ebid_data");
		crc = _cyttsp4_calc_partial_crc(ts,
			pdata, ts->ebid_row_size, crc);
		if (read_back_verify  && (ntable < tsize)) {
			for (i = 0; match && i < ts->ebid_row_size; i++) {
				if (ptable[ntable] != pdata[i]) {
					dev_vdbg(ts->dev,
						"%s: read back err row=%d"
						" table[%d]=%02X"
						" pdata[%d]=%02X\n",
						__func__, row_id,
						ntable, ptable[ntable],
						i, pdata[i]);
					match = false;
				}
				ntable++;
				if (ntable >= tsize) {
					dev_err(ts->dev,
			"%s: row=%d ntbl=%d tsz=%d\n",
						__func__, row_id,
						ntable, tsize);
					break;
				}
			}
		}
		ndata += ts->ebid_row_size;
	}
	/* last row is partial and contains the CRC */
	dev_vdbg(ts->dev,
		"%s: Get CRC bytes for row=%d crc_row=%d\n",
		__func__, crc_row, crc_row);
	retval = _cyttsp4_get_ebid_data_tma400(ts, ebid, crc_row, pdata);
	if (retval < 0) {
		dev_err(ts->dev,
			"%s: Fail get row=%d data r=%d\n",
			__func__, crc_row, retval);
		retval = -EIO;
		goto _cyttsp4_calc_ic_tch_crc_tma400_exit;
	}
	_cyttsp4_pr_buf(ts, pdata, ts->ebid_row_size, "ebid_data");
	crc = _cyttsp4_calc_partial_crc(ts, pdata, crc_ofs, crc);
	ndata += crc_ofs;
	dev_vdbg(ts->dev,
		"%s: ndata=%d\n", __func__, ndata);
	if (read_back_verify  && (ntable < tsize)) {
		dev_vdbg(ts->dev,
			"%s: crc_row=%d ntbl=%d tsz=%d crc_ofs=%d\n",
			__func__, crc_row, ntable, tsize, crc_ofs);
		for (i = 0; match && i < crc_ofs; i++) {
			if (ptable[ntable] != pdata[i]) {
				dev_vdbg(ts->dev,
					"%s: read back err crc_row=%d"
					" table[%d]=%02X"
					" pdata[%d]=%02X\n",
					__func__, crc_row,
					ntable, ptable[ntable],
					i, pdata[i]);
				match = false;
			}
			ntable++;
			if (ntable > tsize) {
				dev_err(ts->dev,
			"%s: crc_row=%d ntbl=%d tsz=%d\n",
					__func__, crc_row, ntable, tsize);
				break;
			}
		}
	}
_cyttsp4_calc_ic_tch_crc_tma400_exit:
	*crc_h = crc / 256;
	*crc_l = crc % 256;

	if (pdata != NULL)
		kfree(pdata);
	if (read_back_verify) {
		if (!match)
			retval = -EIO;
	}
	return retval;
}
#endif /* --CY_USE_TMA400 */
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */

#ifdef CY_USE_TMA884
#ifdef CY_AUTO_LOAD_TOUCH_PARAMS
static int _cyttsp4_calc_settings_crc(struct cyttsp4 *ts, u8 *crc_h, u8 *crc_l)
{
    int retval = 0;
    u8 *buf = NULL;
    u8 size = 0;

    buf = kzalloc(sizeof(uint8_t) * 126, GFP_KERNEL);
    if (buf == NULL) {
        dev_err(ts->dev,"%s: Failed to allocate buf\n", __func__);
        retval = -ENOMEM;
        goto _cyttsp4_calc_settings_crc_exit;
    }

    if (ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL] == NULL) {
        dev_err(ts->dev,"%s: Missing Platform Touch Parameter values table\n",  __func__);
        retval = -ENXIO;
        goto _cyttsp4_calc_settings_crc_exit;
    }
    if ((ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL]->data == NULL) ||
        (ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL]->size == 0)) {
        dev_err(ts->dev,"%s: Missing Platform Touch Parameter values table data\n", __func__);
        retval = -ENXIO;
        goto _cyttsp4_calc_settings_crc_exit;
    }

    size = ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL]->size;

    if (size > 122) {
        dev_err(ts->dev,"%s: Platform data is too large\n", __func__);
        retval = -EOVERFLOW;
        goto _cyttsp4_calc_settings_crc_exit;
    }

    buf[0] = 0x00; /* num of config bytes + 4 high */
    buf[1] = 0x7E; /* num of config bytes + 4 low */
    buf[2] = 0x00; /* max block size w/o crc high */
    buf[3] = 0x7E; /* max block size w/o crc low */

    /* Copy platform data */
    memcpy(&(buf[4]),	ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL]->data,size);

    /* Calculate CRC */
    _cyttsp4_calc_crc(ts, buf, 126, crc_h, crc_l);

_cyttsp4_calc_settings_crc_exit:
    if(buf != NULL)
        kfree(buf);
    return retval;
}
#endif /* --CY_AUTO_LOAD_TOUCH_PARAMS */
#endif /* --CY_USE_TMA884 */

/* Get IC CRC is operational mode command */
static int _cyttsp4_get_ic_crc(struct cyttsp4 *ts,
                               enum cyttsp4_ic_ebid ebid, u8 *crc_h, u8 *crc_l)
{
    int retval = 0;
    u8 cmd_dat[CY_NUM_DAT + 1];	/* +1 for cmd byte */

    memset(cmd_dat, 0, sizeof(cmd_dat));
    cmd_dat[0] = CY_GET_CFG_BLK_CRC;/* pack cmd */
    cmd_dat[1] = ebid;		/* pack EBID id */

    retval = _cyttsp4_put_cmd_wait(ts, ts->si_ofs.cmd_ofs,sizeof(cmd_dat), cmd_dat, CY_HALF_SEC_TMO_MS,
                                   _cyttsp4_chk_cmd_rdy, NULL,ts->platform_data->addr[CY_TCH_ADDR_OFS], true);

    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail Get CRC command r=%d\n",__func__, retval);
        goto _cyttsp4_get_ic_crc_exit;
    }

    memset(cmd_dat, 0, sizeof(cmd_dat));
    retval = _cyttsp4_read_block_data(ts, ts->si_ofs.cmd_ofs,sizeof(cmd_dat), 
                                      cmd_dat,ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail Get CRC status r=%d\n",__func__, retval);
        goto _cyttsp4_get_ic_crc_exit;
    }

    /* Check CRC status and assign values */
    if (cmd_dat[1] != 0) {
        dev_err(ts->dev,"%s: Get CRC status=%d error\n",	__func__, cmd_dat[1]);
        retval = -EIO;
        goto _cyttsp4_get_ic_crc_exit;
    }

    *crc_h = cmd_dat[2];
    *crc_l = cmd_dat[3];

#ifdef CY_USE_TMA400
    retval = _cyttsp4_cmd_handshake(ts);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Command handshake error r=%d\n",__func__, retval);
        /* continue anyway; rely on handshake tmo */
        retval = 0;
    }
#endif /* --CY_USE_TMA400 */

_cyttsp4_get_ic_crc_exit:
    return retval;
}

#ifdef CY_USE_TMA400
static int _cyttsp4_startup(struct cyttsp4 *ts)
{
    int tries;
    int retval = 0;
    u8 ic_crc[2];
#ifdef CY_AUTO_LOAD_TOUCH_PARAMS
    u8 table_crc[2];
#endif /* --CY_AUTO_LOAD_TOUCH_PARAMS */
    bool put_all_params_done = false;
    bool upgraded = false;
    bool fw_upgraded = false;
#ifdef CY_USE_MDDATA_UPDATE
    bool mddata_updated = false;
#endif /* --CY_USE_MDDATA_UPDATE */

#ifdef CYTTSP4_USE_TOUCH_ID
	read_touch_id(ts);
#endif

    tries = 0;
    ts->starting_up = true;
    memset(&ts->test, 0, sizeof(struct cyttsp4_test_mode));
#ifdef CY_USE_WATCHDOG
    _cyttsp4_stop_wd_timer(ts);
#endif
#if defined(CY_AUTO_LOAD_FW) || \
    defined(CY_AUTO_LOAD_TOUCH_PARAMS) || \
    defined(CY_USE_MDDATA_UPDATE)
_cyttsp4_startup_tma400_restart:
#endif /* --CY_AUTO_LOAD_FW || CY_AUTO_LOAD_TOUCH_PARAMS || CY_USE_MDDATA_UPDATE */

    dev_vdbg(ts->dev,"%s: enter driver_state=%d upgraded=%d\n", __func__, ts->driver_state,upgraded);
    ts->current_mode = CY_MODE_BOOTLOADER;
    retval = _cyttsp4_reset(ts);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail reset device r=%d\n", __func__, retval);
        /* continue anyway in case device was already in bootloader */
    }

    /*
     * Wait for heartbeat interrupt. If we didn't get the CPU quickly, this
     * may not be the first interupt.
     */
    dev_vdbg(ts->dev,"%s: wait for first bootloader interrupt\n", __func__);
    retval = _cyttsp4_wait_int(ts, CY_TEN_SEC_TMO_MS);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail waiting for bootloader interrupt\n",__func__);
        goto _cyttsp4_startup_tma400_exit;
    }

    /*
     * exit BL mode and eliminate race between heartbeat and
     * command / response interrupts
     */
    dev_vdbg(ts->dev,"%s: wait for switch, exit bootloader state\n", __func__);
    _cyttsp4_change_state(ts, CY_EXIT_BL_STATE);
    ts->switch_flag = true;
    retval = _cyttsp4_wait_si_int(ts, CY_TEN_SEC_TMO_MS);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail wait switch to Sysinfo r=%d\n",__func__, retval);
        /* continue anyway in case sync missed */
    }
    if (ts->driver_state != CY_SYSINFO_STATE) {
        dev_err(ts->dev,"%s: Fail set sysinfo mode; switch to sysinfo anyway\r",__func__);
        _cyttsp4_change_state(ts, CY_SYSINFO_STATE);
    } else {
        dev_vdbg(ts->dev,"%s: Exit BL ok; now in sysinfo mode\n", __func__);
        _cyttsp4_pr_state(ts);
    }

    dev_vdbg(ts->dev,"%s: initialize sysinfo_ptr\n", __func__);
    _cyttsp4_free_sysinfo_ptr(ts);

    dev_vdbg(ts->dev,"%s: Read Sysinfo regs and get rev numbers try=%d\n",__func__, tries);
    retval = _cyttsp4_get_sysinfo_regs(ts);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Read Block fail -get sys regs (r=%d)\n",__func__, retval);
        dev_err(ts->dev,"%s: Fail to switch from Bootloader to Application r=%d\n",__func__, retval);

        _cyttsp4_change_state(ts, CY_BL_STATE);

        if (upgraded) {
            dev_err(ts->dev,"%s: app failed to launch after platform firmware upgrade\n", __func__);
            retval = -EIO;
            goto _cyttsp4_startup_tma400_exit;
        }

#ifdef CY_AUTO_LOAD_FW
        dev_info(ts->dev,"%s: attempting to reflash IC...\n", __func__);
        if (ts->platform_data->fw->img == NULL ||
            ts->platform_data->fw->size == 0) {
            dev_err(ts->dev,"%s: no platform firmware available for reflashing\n", __func__);
            _cyttsp4_change_state(ts, CY_INVALID_STATE);
            retval = -ENODATA;
            goto _cyttsp4_startup_tma400_exit;
        }
        dev_info(ts->dev,"%s: load firmware image and size.\n", __func__);
        retval = _cyttsp4_load_app(ts,	ts->platform_data->fw->img,ts->platform_data->fw->size);
        if (retval) {
            dev_err(ts->dev,"%s: failed to reflash IC (r=%d)\n",__func__, retval);
            _cyttsp4_change_state(ts, CY_INVALID_STATE);
            retval = -EIO;
            goto _cyttsp4_startup_tma400_exit;
        }
        upgraded = true;
        dev_info(ts->dev,"%s: resetting IC after reflashing\n", __func__);
        goto _cyttsp4_startup_tma400_restart; /* Reset the part */
#endif /* --CY_AUTO_LOAD_FW */
    }

#ifdef CY_AUTO_LOAD_FW
    retval = _cyttsp4_boot_loader(ts, &upgraded);
    if (retval < 0) {
        dev_err(ts->dev, "%s: fail boot loader r=%d)\n", __func__, retval);
        _cyttsp4_change_state(ts, CY_IDLE_STATE);
        goto _cyttsp4_startup_tma400_exit;
    }
    if (upgraded) {
        fw_upgraded = upgraded;
        goto _cyttsp4_startup_tma400_restart;
    }
#endif /* --CY_AUTO_LOAD_FW */

    retval = _cyttsp4_set_mode(ts, CY_CONFIG_MODE);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail set config mode 1 r=%d\n", __func__, retval);
        goto _cyttsp4_startup_tma400_bypass_crc_check;
    }

    retval = _cyttsp4_get_ebid_row_size(ts);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail get EBID row size; using default r=%d\n",__func__, retval);
    }
    dev_vdbg(ts->dev,"%s: get EBID row size=%d\n", __func__, ts->ebid_row_size);

#ifdef CONFIG_TOUCHSCREEN_DEBUG
    if (ts->ic_grptest)
        goto _cyttsp4_startup_tma400_bypass_crc_check;
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */

    memset(ic_crc, 0, sizeof(ic_crc));
    dev_vdbg(ts->dev,"%s: Read IC CRC values\n", __func__);
    /* Get settings CRC from touch IC */
    retval = _cyttsp4_set_mode(ts, CY_OPERATE_MODE);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail set operational mode 1 (r=%d)\n",__func__, retval);
        goto _cyttsp4_startup_tma400_exit;
    }
    retval = _cyttsp4_get_ic_crc(ts, CY_TCH_PARM_EBID, &ic_crc[0], &ic_crc[1]);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail read ic crc r=%d\n",__func__, retval);
    }

    _cyttsp4_pr_buf(ts, ic_crc, sizeof(ic_crc), "read_ic_crc");

    retval = _cyttsp4_set_mode(ts, CY_CONFIG_MODE);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail set config mode 2 r=%d\n", __func__, retval);
        goto _cyttsp4_startup_tma400_exit;
    }

#ifdef CY_AUTO_LOAD_TOUCH_PARAMS
    if (!put_all_params_done) {
        if (ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL] == NULL) {
            dev_err(ts->dev,"%s: missing param table\n", __func__);
            goto _cyttsp4_startup_tma400_bypass_crc_check;
        } else if (ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL]->data == NULL) {
            dev_err(ts->dev,"%s: missing param table data\n", __func__);
            goto _cyttsp4_startup_tma400_bypass_crc_check;
        } else if (ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL]->size == 0) {
            dev_err(ts->dev,"%s: param values table size is 0\n", __func__);
            goto _cyttsp4_startup_tma400_bypass_crc_check;
        }
        _cyttsp_read_table_crc(ts, ts->platform_data->sett[CY_IC_GRPNUM_TCH_PARM_VAL]->data,&table_crc[0], &table_crc[1]);
        /* ic_crc[0] and ic_crc[1] are reversed??? Hmm... ic_crc and table_crc are reversed!! WTF!! */
		dev_info(ts->dev,"%s: ic_crc=%02x%02x table_crc=%02x%02x\n",__func__,
                 ic_crc[0], ic_crc[1],
                 table_crc[0], table_crc[1]);
        _cyttsp4_pr_buf(ts, table_crc, sizeof(table_crc),"read_table_crc");
        if ((ic_crc[1] != table_crc[0]) ||
		    (ic_crc[0] != table_crc[1])) {
            retval = _cyttsp4_put_all_params_tma400(ts);
            if (retval < 0) {
                dev_err(ts->dev,"%s: Fail put all params r=%d\n",__func__, retval);
                goto _cyttsp4_startup_tma400_bypass_crc_check;
            }
            put_all_params_done = true;
            goto _cyttsp4_startup_tma400_restart;
        }
    }
#else
//++ p11309 - 2012.08.13; if not defined CY_AUTO_LOAD_TOUCH_PARAMS, don't calibrate.
	//put_all_params_done = true;
//-- p11309
#endif /* --CY_AUTO_LOAD_TOUCH_PARAMS */

_cyttsp4_startup_tma400_bypass_crc_check:
#ifdef CY_USE_MDDATA_UPDATE
    if (!mddata_updated) {
        retval = _cyttsp4_check_mddata_tma400(ts, &mddata_updated);
        if (retval < 0) {
            dev_err(ts->dev,"%s: Fail update MDDATA r=%d\n",__func__, retval);
        } else if (mddata_updated)
            goto _cyttsp4_startup_tma400_restart;
    }
#endif /* --CY_USE_MDDATA_UPDATE */
	
    if(put_all_params_done || fw_upgraded) {
	    msleep(50);
        cyttsp4_manual_calibration(ts);
        msleep(50);
        cyttsp4_manual_calibration(ts);
        msleep(50);
        cyttsp4_manual_calibration(ts);
    }

//++ p11309 - 2012.08.13; for change operate mode after calibration.
	dbg("finger init & set CY_OPERATE_MODE\n");
#ifdef CY_USE_PROTOCOL_TYPE_B	
	_cyttsp4_clear_finger_info(ts);
	input_mt_destroy_slots(ts->input);
	input_mt_init_slots(ts->input, CY_MAX_FINGERNUM);
#endif /* --CY_USE_PROTOCOL_TYPE_B */	
	/* mode=operational mode, state = active_state */
	retval = _cyttsp4_set_mode(ts, CY_OPERATE_MODE);
	if (retval < 0) {
		dev_err(ts->dev,"%s: Fail set operational mode 2 (r=%d)\n",__func__, retval);
        goto _cyttsp4_startup_tma400_exit;
	}    
//-- p11309

    if (ts->was_suspended) {
        ts->was_suspended = false;
        retval = _cyttsp4_enter_sleep(ts);
        if (retval < 0) {
            dev_err(ts->dev,"%s: fail resume sleep r=%d\n",__func__, retval);
        }
	} else {
#ifdef CY_USE_WATCHDOG
		_cyttsp4_start_wd_timer(ts);
#endif
    }

_cyttsp4_startup_tma400_exit:
    ts->starting_up = false;
    return retval;
}
#endif /* --CY_USE_TMA400 */

#ifdef CY_USE_TMA884
#define CY_IRQ_DEASSERT	1
#define CY_IRQ_ASSERT	0
static int _cyttsp4_startup(struct cyttsp4 *ts)
{
    int retval = 0;
    int i = 0;
    u8 pdata_crc[2];
    u8 ic_crc[2];
    bool upgraded = false;
    bool mddata_updated = false;
    bool wrote_sysinfo_regs = false;
    bool wrote_settings = false;

    memset(&ts->test, 0, sizeof(struct cyttsp4_test_mode));
#ifdef CY_USE_WATCHDOG
    _cyttsp4_stop_wd_timer(ts);
#endif
_cyttsp4_startup_start:
    memset(pdata_crc, 0, sizeof(pdata_crc));
    memset(ic_crc, 0, sizeof(ic_crc));
    dev_vdbg(ts->dev,"%s: enter driver_state=%d\n", __func__, ts->driver_state);
    _cyttsp4_change_state(ts, CY_BL_STATE);

    retval = _cyttsp4_reset(ts);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail reset device r=%d\n", __func__, retval);
        /* continue anyway in case device was already in bootloader */
    }

    /* wait for interrupt to set ready completion */
    retval = _cyttsp4_wait_int(ts, CY_TEN_SEC_TMO_MS);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail waiting for bootloader interrupt\n",__func__);
        goto _cyttsp4_startup_exit;
    }

    INIT_COMPLETION(ts->si_int_running);
    _cyttsp4_change_state(ts, CY_EXIT_BL_STATE);
    ts->switch_flag = true;
    retval = _cyttsp4_wait_si_int(ts, CY_TEN_SEC_TMO_MS);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail wait switch to Sysinfo r=%d\n",__func__, retval);
        /* continue anyway in case sync missed */
    }
    if (ts->driver_state != CY_SYSINFO_STATE)
        _cyttsp4_change_state(ts, CY_SYSINFO_STATE);
    else
        _cyttsp4_pr_state(ts);

    /*
     * TODO: remove this wait for toggle high when
     * startup from ES10 firmware is no longer required
     */
    /* Wait for IRQ to toggle high */
    dev_vdbg(ts->dev,"%s: wait for irq toggle high\n", __func__);
    retval = -ETIMEDOUT;
    for (i = 0; i < CY_DELAY_MAX * 10 * 5; i++) {
        if (ts->platform_data->irq_stat() == CY_IRQ_DEASSERT) {
            retval = 0;
            break;
        }
        mdelay(CY_DELAY_DFLT);
    }
    if (retval < 0) {
        dev_err(ts->dev,"%s: timeout waiting for irq to de-assert\n",__func__);
        goto _cyttsp4_startup_exit;
    }

    dev_vdbg(ts->dev,"%s: read sysinfo 1\n", __func__);
    memset(&ts->sysinfo_data, 0,sizeof(struct cyttsp4_sysinfo_data));
    retval = _cyttsp4_read_block_data(ts, CY_REG_BASE,
                                      sizeof(struct cyttsp4_sysinfo_data), &ts->sysinfo_data,
                                      ts->platform_data->addr[CY_TCH_ADDR_OFS], true);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Fail to switch from Bootloader to Application r=%d\n",	__func__, retval);

        _cyttsp4_change_state(ts, CY_BL_STATE);

        if (upgraded) {
            dev_err(ts->dev,"%s: app failed to launch after platform firmware upgrade\n", __func__);
            retval = -EIO;
            goto _cyttsp4_startup_exit;
        }

#ifdef CY_AUTO_LOAD_FW
        dev_info(ts->dev,	"%s: attempting to reflash IC...\n", __func__);
        if (ts->platform_data->fw->img == NULL ||
            ts->platform_data->fw->size == 0) {
            dev_err(ts->dev,"%s: no platform firmware available for reflashing\n", __func__);
            _cyttsp4_change_state(ts, CY_INVALID_STATE);
            retval = -ENODATA;
            goto _cyttsp4_startup_exit;
        }
        retval = _cyttsp4_load_app(ts,	ts->platform_data->fw->img,ts->platform_data->fw->size);
        if (retval) {
            dev_err(ts->dev,"%s: failed to reflash IC (r=%d)\n",__func__, retval);
            _cyttsp4_change_state(ts, CY_INVALID_STATE);
            retval = -EIO;
            goto _cyttsp4_startup_exit;
        }
        upgraded = true;
        dev_info(ts->dev,"%s: resetting IC after reflashing\n", __func__);
        goto _cyttsp4_startup_start; /* Reset the part */
#endif /* --CY_AUTO_LOAD_FW */
    }

    dev_vdbg(ts->dev,"%s: initialize sysinfo_ptr\n", __func__);
    _cyttsp4_free_sysinfo_ptr(ts);

    /*
     * read system information registers
     * get version numbers and fill sysinfo regs
     */
    dev_vdbg(ts->dev,"%s: Read Sysinfo regs and get version numbers\n", __func__);
    retval = _cyttsp4_get_sysinfo_regs(ts);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Read Block fail -get sys regs (r=%d)\n",__func__, retval);
        _cyttsp4_change_state(ts, CY_IDLE_STATE);
        goto _cyttsp4_startup_exit;
    }

#ifdef CY_AUTO_LOAD_FW
    retval = _cyttsp4_boot_loader(ts, &upgraded);
    if (retval < 0) {
        dev_err(ts->dev,
        "%s: fail boot loader r=%d)\n",
            __func__, retval);
        _cyttsp4_change_state(ts, CY_IDLE_STATE);
        goto _cyttsp4_startup_exit;
    }
    if (upgraded)
        goto _cyttsp4_startup_start;
#endif /* --CY_AUTO_LOAD_FW */

    if (!wrote_sysinfo_regs) {
        dev_vdbg(ts->dev,"%s: Set Sysinfo regs\n", __func__);
        retval = _cyttsp4_set_mode(ts, CY_SYSINFO_MODE);
        if (retval < 0) {
            dev_err(ts->dev,"%s: Set SysInfo Mode fail r=%d\n",__func__, retval);
            _cyttsp4_change_state(ts, CY_IDLE_STATE);
            goto _cyttsp4_startup_exit;
        }
        retval = _cyttsp4_set_sysinfo_regs(ts, &mddata_updated);
        if (retval < 0) {
            dev_err(ts->dev,"%s: Set SysInfo Regs fail r=%d\n",__func__, retval);
            _cyttsp4_change_state(ts, CY_IDLE_STATE);
            goto _cyttsp4_startup_exit;
        } else
            wrote_sysinfo_regs = true;
    }

    dev_vdbg(ts->dev,"%s: enter operational mode\n", __func__);
    retval = _cyttsp4_set_mode(ts, CY_OPERATE_MODE);
    if (retval < 0) {
        _cyttsp4_change_state(ts, CY_IDLE_STATE);
        dev_err(ts->dev,"%s: Fail set operational mode (r=%d)\n",__func__, retval);
        goto _cyttsp4_startup_exit;
    } else {
#ifdef CY_AUTO_LOAD_TOUCH_PARAMS
        /* Calculate settings CRC from platform settings */
        dev_vdbg(ts->dev,"%s: Calculate settings CRC and get IC CRC\n",__func__);
        retval = _cyttsp4_calc_settings_crc(ts,	&pdata_crc[0], &pdata_crc[1]);
        if (retval < 0) {
            dev_err(ts->dev,"%s: Unable to calculate settings CRC\n",__func__);
            goto _cyttsp4_startup_exit;
        }

        /* Get settings CRC from touch IC */
        retval = _cyttsp4_get_ic_crc(ts, CY_TCH_PARM_EBID,&ic_crc[0], &ic_crc[1]);
        if (retval < 0) {
            dev_err(ts->dev,"%s: Unable to get settings CRC\n", __func__);
            goto _cyttsp4_startup_exit;
        }

        /* Compare CRC values */
        dev_info(ts->dev,"%s: PDATA CRC = 0x%02X%02X, IC CRC = 0x%02X%02X\n",
                 __func__, pdata_crc[0], pdata_crc[1],	ic_crc[0], ic_crc[1]);

        if ((pdata_crc[0] == ic_crc[0]) &&
		    (pdata_crc[1] == ic_crc[1]))
            goto _cyttsp4_startup_settings_valid;

        /* Update settings */
        dev_info(ts->dev,"%s: Updating IC settings...\n", __func__);

        if (wrote_settings) {
            dev_err(ts->dev,"%s: Already updated IC settings\n",__func__);
            goto _cyttsp4_startup_settings_valid;
        }

        retval = _cyttsp4_set_op_params(ts, pdata_crc[0], pdata_crc[1]);
        if (retval < 0) {
            dev_err(ts->dev,"%s: Set Operational Params fail r=%d\n",__func__, retval);
            goto _cyttsp4_startup_exit;
        }

        wrote_settings = true;
#else
        wrote_settings = false;
#endif /* --CY_AUTO_LOAD_TOUCH_PARAMS */
    }

#ifdef CY_AUTO_LOAD_TOUCH_PARAMS
_cyttsp4_startup_settings_valid:
#endif /* --CY_AUTO_LOAD_TOUCH_PARAMS */
    if (mddata_updated || wrote_settings) {
        dev_info(ts->dev,"%s: Resetting IC after writing settings\n",__func__);
        mddata_updated = false;
        wrote_settings = false;
        goto _cyttsp4_startup_start; /* Reset the part */
    }
    dev_vdbg(ts->dev,"%s: enable handshake\n", __func__);
    retval = _cyttsp4_handshake_enable(ts);
    if (retval < 0)
        dev_err(ts->dev,"%s: fail enable handshake r=%d", __func__, retval);

    _cyttsp4_change_state(ts, CY_ACTIVE_STATE);

    if (ts->was_suspended) {
        ts->was_suspended = false;
        retval = _cyttsp4_enter_sleep(ts);
        if (retval < 0) {
            dev_err(ts->dev,"%s: fail resume sleep r=%d\n",__func__, retval);
        }
	} else {
#ifdef CY_USE_WATCHDOG
		_cyttsp4_start_wd_timer(ts);
#endif
    }

_cyttsp4_startup_exit:
    return retval;
}
#endif /* --CY_USE_TMA884 */

static irqreturn_t cyttsp4_irq(int irq, void *handle)
{
    struct cyttsp4 *ts = handle;
    u8 rep_stat = 0;
    int retval = 0;

//++ p11309 - 2012.09.24 for IRQ Handler crash
	if ( ts == NULL ) {
		dbg("cyttsp4_irq: ts is NULL\n");
		ts = cyttsp4_data;
	}
//-- p11309

    mutex_lock(&ts->data_lock);

    switch (ts->driver_state) {
    case CY_BL_STATE:
    case CY_CMD_STATE:
        complete(&ts->int_running);
#ifdef CY_USE_LEVEL_IRQ
        udelay(1000);
#endif
        break;
    case CY_SYSINFO_STATE:
        complete(&ts->si_int_running);
#ifdef CY_USE_LEVEL_IRQ
        udelay(500);
#endif
        break;
    case CY_EXIT_BL_STATE:
#ifdef CY_USE_LEVEL_IRQ
        udelay(1000);
#endif
        if (ts->switch_flag == true) {
            ts->switch_flag = false;
            retval = _cyttsp4_ldr_exit(ts);
            if (retval < 0) {
                dev_err(ts->dev,"%s: Fail bl exit r=%d\n",	__func__, retval);
            } else
                ts->driver_state = CY_SYSINFO_STATE;
        }
        break;
    case CY_SLEEP_STATE:
        dev_vdbg(ts->dev,"%s: Attempt to process touch after enter sleep or" " unexpected wake event\n", __func__);
        retval = _cyttsp4_wakeup(ts); /* in case its really asleep */
        if (retval < 0) {
            dev_err(ts->dev,"%s: wakeup fail r=%d\n",__func__, retval);
            _cyttsp4_pr_state(ts);
            _cyttsp4_queue_startup(ts, true);
            break;
        }
        /* Put the part back to sleep */
        retval = _cyttsp4_enter_sleep(ts);
        if (retval < 0) {
            dev_err(ts->dev,"%s: fail resume sleep r=%d\n",__func__, retval);
            _cyttsp4_pr_state(ts);
            _cyttsp4_queue_startup(ts, true);
        }
        break;
    case CY_IDLE_STATE:
        if (ts->xy_mode == NULL) {
            /* initialization is not complete; invalid pointers */
            break;
        }

        /* device now available; signal initialization */
        if(printk_ratelimit())
            dev_info(ts->dev,"%s: Received IRQ in IDLE state\n",__func__);
        /* Try to determine the IC's current state */
        retval = _cyttsp4_load_status_regs(ts);
        if (retval < 0) {
            dev_err(ts->dev,"%s: Still unable to access IC after IRQ r=%d\n",__func__, retval);
            break;
        }
        rep_stat = ts->xy_mode[ts->si_ofs.rep_ofs + 1];
        if (IS_BOOTLOADERMODE(rep_stat)) {
            if(printk_ratelimit())
                dev_info(ts->dev,	"%s: BL mode found in IDLE state\n",	__func__);
            _cyttsp4_queue_startup(ts, false);
            break;
        }
        dev_err(ts->dev,"%s: interrupt received in IDLE state -"" try processing touch\n",	__func__);
        _cyttsp4_change_state(ts, CY_ACTIVE_STATE);
#ifdef CY_USE_WATCHDOG
        _cyttsp4_start_wd_timer(ts);
#endif
        retval = _cyttsp4_xy_worker(ts);
        if (retval < 0) {
            dev_err(ts->dev,"%s: xy_worker IDLE fail r=%d\n",	__func__, retval);
            _cyttsp4_queue_startup(ts, false);
            break;
        }

#ifdef CY_USE_LEVEL_IRQ
        udelay(500);
#endif
        break;
    case CY_READY_STATE:
        complete(&ts->ready_int_running);
        /* do not break; do worker */
    case CY_ACTIVE_STATE:
        if (ts->test.cur_mode == CY_TEST_MODE_CAT) {
            complete(&ts->int_running);
#ifdef CY_USE_LEVEL_IRQ
            udelay(500);
#endif
        } else {
            /* process the touches */
            retval = _cyttsp4_xy_worker(ts);
            if (retval < 0) {
                dev_err(ts->dev,"%s: XY Worker fail r=%d\n",__func__, retval);
                _cyttsp4_queue_startup(ts, false);
            }
        }
        break;
    default:
        break;
    }

//++ p11309 - 2012.09.24 for IRQ Handler crash
	if ( ts == NULL ) {
		dbg("cyttsp4_irq: ts is NULL\n");
		ts = cyttsp4_data;
	}
//-- p11309

    mutex_unlock(&ts->data_lock);

    return IRQ_HANDLED;
}

static int cyttsp4_open(struct input_dev *dev)
{
    int retval = 0;

    struct cyttsp4 *ts = input_get_drvdata(dev);
    dev_dbg(ts->dev, "%s: Open call ts=%p\n", __func__, ts);
    mutex_lock(&ts->data_lock);
    if (!ts->powered) {
        /*
         * execute complete startup procedure.  After this
         * call the device is in active state and the worker
         * is running
         */

#ifdef CY_USE_PM8921
        retval = cyttsp4_init_hw_setting(ts->dev);
        if (retval < 0) {
            _cyttsp4_change_state(ts, CY_IDLE_STATE);
            dev_err(ts->dev,"%s: Init HW setting r=%d\n", __func__, retval);
        }
#endif /* --CY_USE_PM8921 */

        retval = _cyttsp4_startup(ts);

        /* powered if no hard failure */
        if (retval < 0) {
            ts->powered = false;
            _cyttsp4_change_state(ts, CY_IDLE_STATE);
            dev_err(ts->dev,"%s: startup fail at power on r=%d\n",__func__, retval);
        } else {
            ts->powered = true;
        }
        dev_vdbg(ts->dev,"%s: Powered ON(%d) r=%d\n",__func__, (int)ts->powered, retval);
    }
    mutex_unlock(&ts->data_lock);
    return 0;
}

static void cyttsp4_close(struct input_dev *dev)
{
    /*
     * close() normally powers down the device
     * this call simply returns unless power
     * to the device can be controlled by the driver
     */
    return;
}

void cyttsp4_core_release(void *handle)
{
    struct cyttsp4 *ts = handle;

    dev_dbg(ts->dev, "%s: Release call ts=%p\n",__func__, ts);
    if (ts == NULL) {
        dev_err(ts->dev,"%s: Null context pointer on driver release\n",__func__);
        goto cyttsp4_core_release_exit;
    }
#ifdef CY_USE_TOUCH_MONITOR
    touch_monitor_exit();
#endif /* --CY_USE_TOUCH_MONITOR */
#ifdef SKY_PROCESS_CMD_KEY
    touch_fops_exit();
#endif /* --SKY_PROCESS_CMD_KEY */
#ifdef CONFIG_HAS_EARLYSUSPEND
    unregister_early_suspend(&ts->early_suspend);
#endif
	_cyttsp4_file_free(ts);
	if (mutex_is_locked(&ts->data_lock))
		mutex_unlock(&ts->data_lock);
	mutex_destroy(&ts->data_lock);
	free_irq(ts->irq, ts);
    input_unregister_device(ts->input);
    if (ts->cyttsp4_wq) {
        destroy_workqueue(ts->cyttsp4_wq);
        ts->cyttsp4_wq = NULL;
    }

    if (ts->sysinfo_ptr.cydata != NULL)
        kfree(ts->sysinfo_ptr.cydata);
    if (ts->sysinfo_ptr.test != NULL)
        kfree(ts->sysinfo_ptr.test);
    if (ts->sysinfo_ptr.pcfg != NULL)
        kfree(ts->sysinfo_ptr.pcfg);
    if (ts->sysinfo_ptr.opcfg != NULL)
        kfree(ts->sysinfo_ptr.opcfg);
#ifdef CY_USE_DDATA // unused
    if (ts->sysinfo_ptr.ddata != NULL)
        kfree(ts->sysinfo_ptr.ddata);
#endif /* --CY_USE_DDATA */
#ifdef CY_USE_MDATA // unused
    if (ts->sysinfo_ptr.mdata != NULL)
        kfree(ts->sysinfo_ptr.mdata);
#endif /* --CY_USE_MDATA */
    if (ts->xy_mode != NULL)
        kfree(ts->xy_mode);
    if (ts->xy_data != NULL)
        kfree(ts->xy_data);
    if (ts->xy_data_touch1 != NULL)
        kfree(ts->xy_data_touch1);

    kfree(ts);
cyttsp4_core_release_exit:
    return;
}
EXPORT_SYMBOL_GPL(cyttsp4_core_release);

void *cyttsp4_core_init(struct cyttsp4_bus_ops *bus_ops, struct device *dev, int *irq, char *name)
{
    unsigned long irq_flags = 0;
    int i = 0;

    int min = 0;
    int max = 0;
    u16 signal = 0;
    int retval = 0;

    struct input_dev *input_device = NULL;

    struct cyttsp4 *ts = NULL;

    if (dev == NULL) {
        pr_err("%s: Error, dev pointer is Null\n", __func__);
        goto error_alloc_data;
    }

    if (bus_ops == NULL) {
        pr_err("%s: Error, bus_ops Pointer is Null\n", __func__);
        goto error_alloc_data;
    }
    ts = kzalloc(sizeof(*ts), GFP_KERNEL);
    if (ts == NULL) {
        pr_err("%s: Error, kzalloc context memory\n", __func__);
        goto error_alloc_data;
    }

#if defined(CY_USE_FORCE_LOAD) || defined(CONFIG_TOUCHSCREEN_DEBUG)
    ts->fwname = kzalloc(CY_BL_FW_NAME_SIZE, GFP_KERNEL);
    if (ts->fwname == NULL) {
        pr_err("%s: Error, kzalloc fwname\n", __func__);
        goto error_alloc_failed;
    }
#endif /* --CY_USE_FORCE_LOAD || CONFIG_TOUCHSCREEN_DEBUG */

    ts->cyttsp4_wq = create_singlethread_workqueue("cyttsp4_resume_startup_wq");
    if (ts->cyttsp4_wq == NULL) {
        pr_err("%s: No memory for cyttsp4_resume_startup_wq\n",__func__);
        goto error_alloc_failed;
    }

    ts->driver_state = CY_INVALID_STATE;
    ts->current_mode = CY_MODE_BOOTLOADER;
    ts->powered = false;
    ts->was_suspended = false;
    ts->switch_flag = false;
    ts->soft_reset_asserted = false;
    ts->num_prv_tch = 0;
    ts->charger_mode = 0;
    ts->sysinfo_ptr.cydata = NULL;
    ts->sysinfo_ptr.test = NULL;
    ts->sysinfo_ptr.pcfg = NULL;
    ts->sysinfo_ptr.opcfg = NULL;
    ts->sysinfo_ptr.ddata = NULL;
    ts->sysinfo_ptr.mdata = NULL;

    ts->xy_data = NULL;
    ts->xy_mode = NULL;
    ts->xy_data_touch1 = NULL;
    ts->btn_rec_data = NULL;
    memset(&ts->test, 0, sizeof(struct cyttsp4_test_mode));

    ts->dev = dev;
    ts->bus_ops = bus_ops;

#ifdef CONFIG_TOUCHSCREEN_DEBUG
    ts->bus_ops->tsdebug = CY_DBG_LVL_1;
#endif /* --CONFIG_TOUCHSCREEN_DEBUG */    

#ifdef CYTTSP4_USE_TOUCH_ID	
	read_touch_id(ts);
#endif

    if (ts->platform_data == NULL) {
        dev_err(ts->dev,"%s: Error, platform data is Null\n", __func__);
        goto error_alloc_failed;
    }

    if (ts->platform_data->frmwrk == NULL) {
        dev_err(ts->dev,"%s: Error, platform data framework is Null\n",__func__);
        goto error_alloc_failed;
    }

    if (ts->platform_data->frmwrk->abs == NULL) {
        dev_err(ts->dev,"%s: Error, platform data framework array is Null\n",__func__);
        goto error_alloc_failed;
    }

    mutex_init(&ts->data_lock);
    init_completion(&ts->int_running);
    init_completion(&ts->si_int_running);
    init_completion(&ts->ready_int_running);
    ts->flags = ts->platform_data->flags;
#if defined(CY_USE_FORCE_LOAD) || defined(CONFIG_TOUCHSCREEN_DEBUG)
    ts->waiting_for_fw = false;
#endif /* --CY_USE_FORCE_LOAD || CONFIG_TOUCHSCREEN_DEBUG */

#ifdef CY_USE_TMA400
    ts->max_config_bytes = CY_TMA400_MAX_BYTES;
#endif /* --CY_USE_TMA400 */
#ifdef CY_USE_TMA884
    ts->max_config_bytes = CY_TMA884_MAX_BYTES;
#endif /* --CY_USE_TMA884 */
   
    *irq = gpio_to_irq(GPIO_TOUCH_CHG);
    ts->irq = *irq;
    if (ts->irq <= 0) {
        dev_vdbg(ts->dev,"%s: Error, failed to allocate irq\n", __func__);
        goto error_init;
    }

    /* Create the input device and register it. */
    dev_vdbg(ts->dev,"%s: Create the input device and register it\n", __func__);
    input_device = input_allocate_device();
    if (input_device == NULL) {
        dev_err(ts->dev,"%s: Error, failed to allocate input device\n",__func__);
        goto error_init;
    }

    ts->input = input_device;
    input_device->name = name;
    snprintf(ts->phys, sizeof(ts->phys)-1, "%s", dev_name(dev));
    input_device->phys = ts->phys;
    input_device->dev.parent = ts->dev;
    ts->bus_type = bus_ops->dev->bus;

//++ p11309 - 2012.08.12 for resource sharing with cyttsp4_core.c and cyttsp4_pantech.h
	cyttsp4_data = ts;
//-- p11309

#ifdef CY_USE_WATCHDOG
    INIT_WORK(&ts->work, cyttsp4_timer_watchdog);
    setup_timer(&ts->timer, cyttsp4_timer, (unsigned long)ts);
#endif	

    input_device->open = cyttsp4_open;
    input_device->close = cyttsp4_close;
    input_set_drvdata(input_device, ts);
    dev_set_drvdata(dev, ts);

    dev_vdbg(ts->dev,"%s: Initialize event signals\n", __func__);
    set_bit(EV_ABS, input_device->evbit);
    set_bit(EV_KEY, input_device->evbit);
    set_bit(EV_SYN, input_device->evbit);
    set_bit(INPUT_PROP_DIRECT, input_device->propbit);
	set_bit(BTN_TOUCH, input_device->keybit);

#ifdef CY_USE_PROTOCOL_TYPE_B
//++ p11309
#if 0
	for (i=0; i<CY_NUM_TRK_ID;i++) finger_info[i].abs[CY_TCH_T] = -1;
	input_mt_init_slots(input_device, CY_NUM_TRK_ID);	
#else
	_pantech_reset_fingerinfo();
	input_mt_init_slots(input_device, CY_MAX_FINGERNUM);
#endif    
//-- p1130
#endif /* --CY_USE_PROTOCOL_TYPE_B */

    for (i = 0; i < (ts->platform_data->frmwrk->size / CY_NUM_ABS_SET); i++) {
        signal = ts->platform_data->frmwrk->abs[(i * CY_NUM_ABS_SET) + CY_SIGNAL_OST];
        if (signal != CY_IGNORE_VALUE) {
            min = ts->platform_data->frmwrk->abs[(i * CY_NUM_ABS_SET) + CY_MIN_OST];
            max = ts->platform_data->frmwrk->abs[(i * CY_NUM_ABS_SET) + CY_MAX_OST];
            if (i == CY_ABS_ID_OST) {
                /* shift track ids down to start at 0 */
                max = max - min;
                min = min - min;
            }
            input_set_abs_params(input_device,signal,min,max,
                                 ts->platform_data->frmwrk->abs[(i * CY_NUM_ABS_SET) + CY_FUZZ_OST],
                                 ts->platform_data->frmwrk->abs[(i * CY_NUM_ABS_SET) + CY_FLAT_OST]);
        }
    }

#ifdef CY_USE_DEBUG_TOOLS
    if (ts->flags & CY_FLAG_FLIP) {
        input_set_abs_params(input_device,	ABS_MT_POSITION_X,
                             ts->platform_data->frmwrk->abs[(CY_ABS_Y_OST * CY_NUM_ABS_SET) + CY_MIN_OST],
                             ts->platform_data->frmwrk->abs[(CY_ABS_Y_OST * CY_NUM_ABS_SET) + CY_MAX_OST],
                             ts->platform_data->frmwrk->abs[(CY_ABS_Y_OST * CY_NUM_ABS_SET) + CY_FUZZ_OST],
                             ts->platform_data->frmwrk->abs[(CY_ABS_Y_OST * CY_NUM_ABS_SET) + CY_FLAT_OST]);

        input_set_abs_params(input_device,ABS_MT_POSITION_Y,
                             ts->platform_data->frmwrk->abs[(CY_ABS_X_OST * CY_NUM_ABS_SET) + CY_MIN_OST],
                             ts->platform_data->frmwrk->abs[(CY_ABS_X_OST * CY_NUM_ABS_SET) + CY_MAX_OST],
                             ts->platform_data->frmwrk->abs[(CY_ABS_X_OST * CY_NUM_ABS_SET) + CY_FUZZ_OST],
                             ts->platform_data->frmwrk->abs[(CY_ABS_X_OST * CY_NUM_ABS_SET) + CY_FLAT_OST]);
    }
#endif /* --CY_USE_DEBUG_TOOLS */

    //input_set_events_per_packet(input_device, 6 * CY_NUM_TCH_ID);

#ifdef SKY_PROCESS_CMD_KEY
	set_bit(KEY_MENU, input_device->keybit);
	set_bit(KEY_HOME, input_device->keybit);
	set_bit(KEY_BACK, input_device->keybit);
	set_bit(KEY_SEARCH, input_device->keybit);	
	set_bit(KEY_HOMEPAGE, input_device->keybit);

	set_bit(KEY_0, input_device->keybit);
	set_bit(KEY_1, input_device->keybit);
	set_bit(KEY_2, input_device->keybit);
	set_bit(KEY_3, input_device->keybit);
	set_bit(KEY_4, input_device->keybit);
	set_bit(KEY_5, input_device->keybit);
	set_bit(KEY_6, input_device->keybit);
	set_bit(KEY_7, input_device->keybit);
	set_bit(KEY_8, input_device->keybit);
	set_bit(KEY_9, input_device->keybit);
	set_bit(0xe3, input_device->keybit); /* '*' */
	set_bit(0xe4, input_device->keybit); /* '#' */
	set_bit(0xe5, input_device->keybit); /* 'KEY_END' */

	set_bit(KEY_POWER, input_device->keybit);
	set_bit(KEY_LEFTSHIFT, input_device->keybit);
	set_bit(KEY_RIGHTSHIFT, input_device->keybit);
	set_bit(KEY_LEFT, input_device->keybit);
	set_bit(KEY_RIGHT, input_device->keybit);
	set_bit(KEY_UP, input_device->keybit);
	set_bit(KEY_DOWN, input_device->keybit);
	set_bit(KEY_ENTER, input_device->keybit);

	set_bit(KEY_SEND, input_device->keybit);
	set_bit(KEY_END, input_device->keybit);

	set_bit(KEY_F1, input_device->keybit);
	set_bit(KEY_F2, input_device->keybit);
	set_bit(KEY_F3, input_device->keybit);				// P13106 VT_CALL for VT TEST 121019				
	set_bit(KEY_F4, input_device->keybit);

	set_bit(KEY_VOLUMEUP, input_device->keybit);
	set_bit(KEY_VOLUMEDOWN, input_device->keybit);

	set_bit(KEY_CLEAR, input_device->keybit);
	set_bit(KEY_CAMERA, input_device->keybit);
	
    //set_bit(KEY_HOLD, input_device->keybit);
#endif // SKY_PROCESS_CMD_KEY

    dev_vdbg(ts->dev,"%s: Initialize irq\n", __func__);
#ifdef CY_USE_LEVEL_IRQ
    irq_flags = IRQF_TRIGGER_LOW | IRQF_ONESHOT;
#else
    irq_flags = IRQF_TRIGGER_FALLING | IRQF_ONESHOT;
#endif
    retval = request_threaded_irq(ts->irq, NULL, cyttsp4_irq,irq_flags, ts->input->name, ts);
    if (retval < 0) {
        dev_err(ts->dev,"%s: failed to init irq r=%d name=%s\n",__func__, retval, ts->input->name);
        ts->irq_enabled = false;
        goto error_init;
    } else {
        ts->irq_enabled = true;
    }

    retval = input_register_device(input_device);
    if (retval < 0) {
        dev_err(ts->dev,"%s: Error, failed to register input device r=%d\n",__func__, retval);
        goto error_init;
    }

    /* add /sys files */
    _cyttsp4_file_init(ts);

#ifdef CONFIG_HAS_EARLYSUSPEND
    ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
    ts->early_suspend.suspend = cyttsp4_early_suspend;
    ts->early_suspend.resume = cyttsp4_late_resume;
    register_early_suspend(&ts->early_suspend);
#endif

#ifdef CY_USE_TOUCH_MONITOR
    touch_monitor_init();	
#endif /* --CY_USE_TOUCH_MONITOR */
#ifdef SKY_PROCESS_CMD_KEY    
    touch_fops_init();
#endif /* --SKY_PROCESS_CMD_KEY */

    INIT_WORK(&ts->cyttsp4_resume_startup_work, cyttsp4_ts_work_func);

    goto no_error;

error_init:
    mutex_destroy(&ts->data_lock);
    if (ts->cyttsp4_wq) {
        destroy_workqueue(ts->cyttsp4_wq);
        ts->cyttsp4_wq = NULL;
    }
error_alloc_failed:
    if (ts != NULL) {
        kfree(ts);
        ts = NULL;
    }
error_alloc_data:
	dev_err(ts->dev,
			"%s: Failed Initialization\n", __func__);
no_error:
    Pow_ON_T=1;
	
    return ts;
}
EXPORT_SYMBOL_GPL(cyttsp4_core_init);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Cypress TrueTouch(R) Standard touchscreen driver core");
MODULE_AUTHOR("Cypress");
