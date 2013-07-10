/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "msm_sensor.h"
#include <linux/regulator/machine.h> 
#include "sensor_ctrl.h"

#include <media/v4l2-subdev.h>
#include "msm.h"
#include "msm_ispif.h"
#include "msm_camera_i2c.h"
#if 1 // jjhwang File 
#include <linux/syscalls.h>
#endif

#include <linux/gpio.h>

#include "ce1612_v4l2_cfg.h"

#define F_FW_UPDATE
#define F_MIPI2LANE
#define F_STREAM_ON_OFF
//#define I2C_LOG_PRINT

#define ON  1
#define OFF 0

#ifdef F_FW_UPDATE
#define CE1612_UPLOADER_INFO_F   "/CE161F00.bin"
#define CE1612_UPLOADER_BIN_F    "/CE161F01.bin"
#define CE1612_FW_INFO_F         "/CE161F02.bin"
#define CE1612_FW_BIN_F          "/CE161F03.bin"

#endif

#define CE1612_FW_MAJOR_VER	49
#define CE1612_FW_MINOR_VER	7
#define CE1612_PRM_MAJOR_VER	49
#define CE1612_PRM_MINOR_VER	7


struct ce1612_ver_t {
	uint8_t fw_major_ver;
	uint8_t fw_minor_ver;
	uint8_t prm_major_ver;
	uint8_t prm_minor_ver;
};

#ifdef F_FW_UPDATE
static struct ce1612_ver_t ce1612_ver = {0, 0};
#endif

#define SENSOR_NAME "ce1612"
#define PLATFORM_DRIVER_NAME "msm_camera_ce1612"
#define ce1612_obj ce1612_##obj

/*=============================================================
	SENSOR REGISTER DEFINES
==============================================================*/

/* Sensor Model ID */
#define CE1612_PIDH_REG		0x00
#define CE1612_MODEL_ID		1612

//wsyang_temp
#define F_CE1612_POWER

#ifdef F_CE1612_POWER
#define CAMIO_R_RST_N			0
#define CAMIO_R_STB_N			1
#define CAM1_DVDD_EN				2
#define CAM1_AVDD_EN				3
#define CAM1_IOVDD_EN			4
#define CAM1_VDD_EN				5
#define CAM1_1P1V_LOWQ 			6
#define CAM1_INT_N 				7
#define CAMIO_MAX					8

#define CAM1_RST_N		107
#define CAM1_STANDBY		54
#define CAM1_DVDD			58
#define CAM1_AVDD			82
#define CAM1_IOVDD		77
#define CAM1_VDD			3

#define CAM1_LOWQ 		18
#define CAM1_INT			75


static sgpio_ctrl_t sgpios[CAMIO_MAX] = {
	{CAMIO_R_RST_N, "CAM_R_RST_N", CAM1_RST_N},
	{CAMIO_R_STB_N, "CAM_R_STB_N", CAM1_STANDBY},
	{CAM1_DVDD_EN, "CAM1_DVDD_N", CAM1_DVDD},
	{CAM1_AVDD_EN, "CAM1_AVDD_N", CAM1_AVDD},
	{CAM1_IOVDD_EN, "CAM1_IOVDD_N", CAM1_IOVDD},
	{CAM1_VDD_EN, "CAM1_VDD_N", CAM1_VDD},
	{CAM1_1P1V_LOWQ, "CAM1_1P1V_LOW", CAM1_LOWQ},
	{CAM1_INT_N, "CAM_INT_N", CAM1_INT},
};


#define CAMV_VDD_SYSIO_2P8V	0
#define CAMV_MAX	1

static svreg_ctrl_t svregs[CAMV_MAX] = {
	{CAMV_VDD_SYSIO_2P8V, "8921_l16", NULL, 2800},
};
#endif

DEFINE_MUTEX(ce1612_mut);
static struct msm_sensor_ctrl_t ce1612_s_ctrl;

static int32_t ce1612_lens_stop(struct msm_sensor_ctrl_t *s_ctrl);
static int32_t ce1612_set_coninuous_af(struct msm_sensor_ctrl_t *s_ctrl ,int8_t caf);
static int32_t ce1612_lens_stability(struct msm_sensor_ctrl_t *s_ctrl);

static int8_t continuous_af_mode = 0;   	// 0: no caf, 1: af-c, 2: af-t
static int8_t sensor_mode = -1;   			// 0: full size,  1: qtr size, 2: fullhd size, 3: ZSL
static int32_t x1 = 0, y1=0, x2=0, y2=0;  // 0: full size,  1: qtr size, 2: fullhd size
static int8_t g_get_hw_revision = 0;
extern int get_hw_revision(void);
static int8_t zsl_status = 1;


static struct v4l2_subdev_info ce1612_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_YUYV8_2X8,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};


#define C_PANTECH_CAMERA_MIN_PREVIEW_FPS	5
#define C_PANTECH_CAMERA_MAX_PREVIEW_FPS	31

#define CE1612_FULL_SIZE_DUMMY_PIXELS     0
#define CE1612_FULL_SIZE_DUMMY_LINES    	0
#define CE1612_FULL_SIZE_WIDTH    			3264
#define CE1612_FULL_SIZE_HEIGHT   			2448

#define CE1612_QTR_SIZE_DUMMY_PIXELS  	0
#define CE1612_QTR_SIZE_DUMMY_LINES   	0
#define CE1612_QTR_SIZE_WIDTH     		1280 
#define CE1612_QTR_SIZE_HEIGHT    		960 

#define CE1612_HRZ_FULL_BLK_PIXELS   	0
#define CE1612_VER_FULL_BLK_LINES     	0

#define CE1612_HRZ_QTR_BLK_PIXELS    	0
#define CE1612_VER_QTR_BLK_LINES      	0

#define CE1612_1080P_SIZE_WIDTH        1920
#define CE1612_1080P_SIZE_HEIGHT        1088

#define CE1612_HRZ_1080P_BLK_PIXELS      0
#define CE1612_VER_1080P_BLK_LINES        0

#define CE1612_ZSL_SIZE_WIDTH    2560 
#define CE1612_ZSL_SIZE_HEIGHT   1920 


static struct msm_sensor_output_info_t ce1612_dimensions[] = {
	{
		.x_output = CE1612_FULL_SIZE_WIDTH,
		.y_output = CE1612_FULL_SIZE_HEIGHT,
		.line_length_pclk = CE1612_FULL_SIZE_WIDTH + CE1612_HRZ_FULL_BLK_PIXELS ,
		.frame_length_lines = CE1612_FULL_SIZE_HEIGHT+ CE1612_VER_FULL_BLK_LINES ,
		.vt_pixel_clk = 198000000, //207000000, //276824064, 
		.op_pixel_clk = 198000000, //207000000, //276824064, 
		.binning_factor = 1,
	},
	{
		.x_output = CE1612_QTR_SIZE_WIDTH,
		.y_output = CE1612_QTR_SIZE_HEIGHT,
		.line_length_pclk = CE1612_QTR_SIZE_WIDTH + CE1612_HRZ_QTR_BLK_PIXELS,
		.frame_length_lines = CE1612_QTR_SIZE_HEIGHT+ CE1612_VER_QTR_BLK_LINES,
		.vt_pixel_clk = 198000000, //207000000, //276824064, 
		.op_pixel_clk = 198000000, //207000000, //276824064, 
		.binning_factor = 2,
	},
	{

		.x_output = CE1612_1080P_SIZE_WIDTH,
		.y_output = CE1612_1080P_SIZE_HEIGHT,
		.line_length_pclk = CE1612_1080P_SIZE_WIDTH + CE1612_HRZ_1080P_BLK_PIXELS ,
		.frame_length_lines = CE1612_1080P_SIZE_HEIGHT+ CE1612_VER_1080P_BLK_LINES ,
		.vt_pixel_clk = 198000000, //207000000,//276824064,  
		.op_pixel_clk = 198000000, //207000000,//276824064,  
		.binning_factor = 2,
	},
	{
		.x_output = CE1612_ZSL_SIZE_WIDTH,
		.y_output = CE1612_ZSL_SIZE_HEIGHT,
		.line_length_pclk = CE1612_ZSL_SIZE_WIDTH,
		.frame_length_lines = CE1612_ZSL_SIZE_HEIGHT,
		.vt_pixel_clk = 198000000, //207000000,//276824064,  
		.op_pixel_clk = 198000000, //207000000,//276824064, 
		.binning_factor = 1,
	},	
};

static struct msm_camera_csid_vc_cfg ce1612_cid_cfg[] = {
	{0, CSI_YUV422_8, CSI_DECODE_8BIT}, 
	{1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
	{2, CSI_RESERVED_DATA, CSI_DECODE_8BIT},
	{3, CSI_RESERVED_DATA, CSI_DECODE_8BIT},	
};

static struct msm_camera_csi2_params ce1612_csi_params = {
	.csid_params = {
		//ane_assign = 0xe4,
#ifdef F_MIPI2LANE
		.lane_cnt = 2, 
#else
		.lane_cnt = 4, 
#endif
		.lut_params = {
			.num_cid = 2,
			.vc_cfg = ce1612_cid_cfg,
		},
	},
	.csiphy_params = {
#ifdef F_MIPI2LANE
		.lane_cnt = 2,
#else		
		.lane_cnt = 4,
#endif
		.settle_cnt = 0x14,
		
#ifdef F_MIPI2LANE
		//ane_mask = 0x3,
#endif
	},
};

static struct msm_camera_csi2_params *ce1612_csi_params_array[] = {
	&ce1612_csi_params,
	&ce1612_csi_params,
	&ce1612_csi_params,
	&ce1612_csi_params,
};

static struct msm_sensor_id_info_t ce1612_id_info = {
	.sensor_id_reg_addr = CE1612_PIDH_REG,
	.sensor_id = 0,
};


int32_t ce1612_sensor_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id);


static const struct i2c_device_id ce1612_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&ce1612_s_ctrl},
	{ }
};

static struct i2c_driver ce1612_i2c_driver = {
	.id_table = ce1612_i2c_id,
	.probe  = ce1612_sensor_i2c_probe, //msm_sensor_i2c_probe
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ce1612_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_BYTE_ADDR,
};


static int32_t ce1612_cmd(struct msm_sensor_ctrl_t *s_ctrl , uint8_t cmd, uint8_t * ydata, int32_t num_data)
{
	int32_t rc = 0;
	unsigned char buf[130];
	int32_t i = 0;

	memset(buf, 0, sizeof(buf));

	buf[0] = cmd;

#ifdef I2C_LOG_PRINT
	SKYCDBG("++++ I2C W : \n");
	SKYCDBG("0x%x \n", buf[0]);
#endif

	if(ydata != NULL)
	{
		for(i = 0; i < num_data; i++)
		{
			buf[i+1] = *(ydata+i);
#ifdef I2C_LOG_PRINT
			SKYCDBG("0x%x \n", buf[i+1]);	
#endif
		}	
	}

#ifdef I2C_LOG_PRINT
	SKYCDBG("++++\n");
#endif

	rc = msm_camera_i2c_txdata(s_ctrl->sensor_i2c_client, buf, num_data+1);

	if (rc < 0)
		SKYCERR("ERR:%s FAIL!!!cmd=%d , rc=%d return~~\n", __func__, cmd, rc);	

	return rc;
}


#ifdef F_FW_UPDATE
static int32_t ce1612_read(struct msm_sensor_ctrl_t *s_ctrl , uint8_t * rdata, uint32_t len)
{
	int32_t rc = 0;

#ifdef I2C_LOG_PRINT
	int32_t i = 0;
	SKYCDBG("---- I2C R : \n");
#endif

	rc = msm_camera_i2c_rxdata_2(s_ctrl->sensor_i2c_client, rdata, len);

#ifdef I2C_LOG_PRINT
	if(len != 0)
	{
		for(i = 0; i < len; i++)
		{
			SKYCDBG("0x%x \n", *(rdata+i));	
		}	
	}
	SKYCDBG("----\n");
#endif

	return rc;
}
#endif

static int32_t ce1612_cmd_read(struct msm_sensor_ctrl_t *s_ctrl , unsigned char cmd, uint8_t * rdata, uint32_t len)
{
	int32_t rc = 0;

#ifdef I2C_LOG_PRINT
	int32_t i = 0;
	SKYCDBG("++++ I2C W :\n");
	SKYCDBG("0x%x \n", cmd);
	SKYCDBG("++++\n");
	SKYCDBG("---- I2C R : \n");
#endif

	*rdata = cmd;
	rc = msm_camera_i2c_rxdata(s_ctrl->sensor_i2c_client, rdata, len);

#ifdef I2C_LOG_PRINT
	if(len != 0)
	{
		for(i = 0; i < len; i++)
		{
			SKYCDBG("0x%x \n", *(rdata+i));	
		}	
	}
	SKYCDBG("----\n");
#endif

	if(rc >= 0)
		rc = 0;

	return rc;
}


static int32_t ce1612_poll(struct msm_sensor_ctrl_t *s_ctrl , unsigned char cmd, uint8_t pdata, 
			   uint8_t mperiod, uint32_t retry)
{
	unsigned char rdata = 0;
	uint32_t i = 0;
	int32_t rc = 0;
	unsigned char tmp_raddr;

	for (i = 0; i < retry; i++) {
		rc = ce1612_cmd_read(s_ctrl, cmd, &tmp_raddr, 1);
		
		rdata = tmp_raddr;
		
#if 0        
		SKYCDBG("%s: (mperiod=%d, retry=%d) <%d>poll data = 0x%x, read = 0x%x\n", __func__, mperiod, retry, i, pdata, rdata);        
#endif

		if (rdata == pdata)
			break;
		msleep(mperiod);
	}

	if (i == retry) {
		SKYCERR("%s: err(-ETIMEDOUT)\n", __func__);
		rc = -ETIMEDOUT;
		return rc; 
	}

	return 0;
}


static int32_t ce1612_poll_bit(struct msm_sensor_ctrl_t *s_ctrl, uint8_t cmd, uint8_t mperiod, uint32_t retry)
{
	uint8_t rdata = 0;
	uint32_t i = 0;
	int32_t rc = 0;

	for (i = 0; i < retry; i++) {

		rc = ce1612_cmd_read(s_ctrl, cmd, &rdata, 1);
		
#if 0        
		SKYCDBG("%s: (mperiod=%d, retry=%d) <%d> read = 0x%x\n", __func__, mperiod, retry, i, rdata);        
#endif

		if (rc < 0)
			break;
		
		if (!(rdata & 0x01))
			break;
		msleep(mperiod);
	}

	if (i == retry) {
		SKYCERR("%s: err(-ETIMEDOUT)\n", __func__);
		rc = -ETIMEDOUT;
		return rc;//charley2
	}

	return 0;
}



#ifdef F_CE1612_POWER
static int ce1612_vreg_init(void)
{
	int rc = 0;
	SKYCDBG("%s: %d E\n", __func__, __LINE__);

	rc = sgpio_init(sgpios, CAMIO_MAX);
	if (rc < 0) {
		SKYCERR("%s: sgpio_init failed \n", __func__);
		goto sensor_init_fail;
	}

	rc = svreg_init(svregs, CAMV_MAX);
	if (rc < 0) {
		SKYCERR("%s: svreg_init failed \n", __func__);
		goto sensor_init_fail;
	}

	SKYCDBG("%s: X\n",__func__);
	return rc;

sensor_init_fail:
    return -ENODEV;
}


int32_t ce1612_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;

	g_get_hw_revision = get_hw_revision();
	SKYCERR("%s: g_get_hw_revision=%d E\n",__func__, g_get_hw_revision);

#if 0
	msm_sensor_probe_on(&s_ctrl->sensor_i2c_client->client->dev);   //////////////////
#else
	rc = msm_sensor_power_up(s_ctrl);
	SKYCDBG(" %s : msm_sensor_power_up : rc = %d E\n",__func__, rc);  
#endif    
	ce1612_vreg_init();

	if (sgpio_ctrl(sgpios, CAM1_1P1V_LOWQ, 1) < 0)	rc = -EIO;	// VDD (CORE) 1.1V
	if (sgpio_ctrl(sgpios, CAM1_DVDD_EN, 1) < 0)	   rc = -EIO;	// VDD (CORE) 1.1V
	mdelay(3);

	if (sgpio_ctrl(sgpios, CAM1_VDD_EN, 1) < 0)		rc = -EIO;	// VDD_HOSTIO 1.8V 
	if (sgpio_ctrl(sgpios, CAM1_IOVDD_EN, 1) < 0)	rc = -EIO;	// VDD_CAMIO & CAM _IOVDD 1.8V 
	
	if(g_get_hw_revision <= TP10)
		if (sgpio_ctrl(sgpios, CAM1_AVDD_EN, 1) < 0)	   rc = -EIO;	// VDD_SYSIO 2.8V (TP10 : +AF 2.8V)

	if (svreg_ctrl(svregs, CAMV_VDD_SYSIO_2P8V, 1) < 0) rc = -EIO;	// CAM_AVDD 2.8V (TP20 : +VDD_SYSIO 2.8V)
	mdelay(5);

	if (sgpio_ctrl(sgpios, CAMIO_R_STB_N, 1) < 0)	rc = -EIO;	// ISP Standby
	mdelay(1);
	if (sgpio_ctrl(sgpios, CAMIO_R_RST_N, 1) < 0)	rc = -EIO;	// ISP Reset
	
	if(g_get_hw_revision  > TP10)
		if (sgpio_ctrl(sgpios, CAM1_AVDD_EN, 1) < 0)	rc = -EIO;     // AF 2.8V
	mdelay(10);

	continuous_af_mode = 0;
	sensor_mode = -1;
	

	SKYCERR("%s X (%d)\n", __func__, rc);
	return rc;
}


int32_t ce1612_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;

	g_get_hw_revision = get_hw_revision();
	SKYCERR("%s: g_get_hw_revision=%d E\n",__func__, g_get_hw_revision);

	//ce1612_lens_stability(s_ctrl);

#if 0
	msm_sensor_probe_off(&s_ctrl->sensor_i2c_client->client->dev);  //////////////
#else
	msm_sensor_power_down(s_ctrl);
	SKYCDBG(" %s : msm_sensor_power_down : rc = %d E\n",__func__, rc);  
#endif

	if(g_get_hw_revision  > TP10)
		if (sgpio_ctrl(sgpios, CAM1_AVDD_EN, 0) < 0) 	rc = -EIO;	// AF 2.8V

	if (sgpio_ctrl(sgpios, CAMIO_R_RST_N, 0) < 0)	rc = -EIO;	// ISP Reset
	mdelay(1);	
	if (sgpio_ctrl(sgpios, CAMIO_R_STB_N, 0) < 0)	rc = -EIO;	// ISP Standby
	mdelay(2);

	if (svreg_ctrl(svregs, CAMV_VDD_SYSIO_2P8V, 0) < 0)	rc = -EIO;	// CAM_AVDD 2.8V (TP20 : +VDD_SYSIO 2.8V)
	if (sgpio_ctrl(sgpios, CAM1_IOVDD_EN, 0) < 0)	      rc = -EIO;	// VDD_CAMIO & CAM _IOVDD 1.8V  
	if (sgpio_ctrl(sgpios, CAM1_VDD_EN, 0) < 0)		      rc = -EIO;	// VDD_HOSTIO 1.8V 

	if(g_get_hw_revision <= TP10)
		if (sgpio_ctrl(sgpios, CAM1_AVDD_EN, 0) < 0)          rc = -EIO;	// VDD_SYSIO 2.8V (TP10 : +AF 2.8V)
	mdelay(2);

	if (sgpio_ctrl(sgpios, CAM1_1P1V_LOWQ, 0) < 0)	rc = -EIO;	// VDD (CORE) 1.1V
	if (sgpio_ctrl(sgpios, CAM1_DVDD_EN, 0) < 0)	rc = -EIO;	   // VDD (CORE) 1.1V
	mdelay(10);

	svreg_release(svregs, CAMV_MAX);
	sgpio_release(sgpios, CAMIO_MAX);

	SKYCERR("%s X (%d)\n", __func__, rc);
	return rc;
}
#endif


#ifdef F_FW_UPDATE
#define CE1612_FW_INFO_W_CMD 0xF2
#define CE1612_FW_DATA_W_CMD 0xF4
#define CE1612_FW_DATA_WP_CMD 0xF3
#define CE1612_NUM_CFG_CMD 4
#define CE1612_NUM_UPDATE_DATA 129
#define CE1612_PACKETS_IN_TABLE 508

static int32_t ce1612_update_fw(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	uint32_t numPacket = 0;
	int fd = 0;

	uint8_t pFW[CE1612_NUM_UPDATE_DATA+1];
	uint32_t i = 0;
	uint8_t rdata = 0xC0;
	uint8_t *pcmd = &pFW[0];
	uint8_t *pdata = &pFW[1];

	mm_segment_t old_fs; 

	old_fs = get_fs();
	set_fs(KERNEL_DS); 

	fd = sys_open(CE1612_UPLOADER_INFO_F, O_RDONLY, 0);
	if (fd < 0) {
		SKYCERR("%s: Can not open %s\n", __func__, CE1612_UPLOADER_INFO_F);
		goto fw_update_fail;
	}

	if (sys_read(fd, (char *)pdata, CE1612_NUM_CFG_CMD) != CE1612_NUM_CFG_CMD) {
		SKYCERR("%s: Can not read %s\n", __func__, CE1612_UPLOADER_INFO_F);
		sys_close(fd);
		goto fw_update_fail;
	}

	numPacket = (*(pdata+1) & 0xFF) << 8;
	numPacket |= *pdata & 0xFF;

	SKYCDBG("%s start : number of uploader packets is 0x%x\n",__func__, numPacket);

	*pcmd= CE1612_FW_INFO_W_CMD;
	//	rc = ce1612_cmd(s_ctrl, cmd, pFW, CE1612_NUM_CFG_CMD);
	rc = msm_camera_i2c_txdata(s_ctrl->sensor_i2c_client, pFW, CE1612_NUM_CFG_CMD+1);
	if (rc < 0)
	{
		SKYCERR("%s : uploader configs write ERROR 0x%x, 0x%x, 0x%x, 0x%x, 0x%x!!!\n",__func__, pFW[0], pFW[1], pFW[2], pFW[3], pFW[4]);
		sys_close(fd);
		goto fw_update_fail;
	}
	sys_close(fd);
	SKYCDBG("%s : fw uploader info write OK !!!!\n",__func__);
	
	/////////////////////////////////////////////////////////////////////////////////

	fd = sys_open(CE1612_UPLOADER_BIN_F, O_RDONLY, 0);

	if (fd < 0) {
		SKYCERR("%s: Can not open %s\n", __func__, CE1612_UPLOADER_BIN_F);
		goto fw_update_fail;
	}

	for(i = 0; i < numPacket; i++)
	{
		if (sys_read(fd, (char *)pdata, CE1612_NUM_UPDATE_DATA) != CE1612_NUM_UPDATE_DATA) {
			SKYCERR("%s: Can not read %s : %d packet \n", __func__, CE1612_UPLOADER_BIN_F, i);
			sys_close(fd);
			goto fw_update_fail;
		}

		*pcmd= CE1612_FW_DATA_W_CMD;
		//	rc = ce1612_cmd(s_ctrl, cmd, pFW, CE1612_NUM_CFG_CMD);
		rc = msm_camera_i2c_txdata(s_ctrl->sensor_i2c_client, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : uploader packet %d write ERROR !!!\n",__func__, i);
			sys_close(fd);
			goto fw_update_fail;
		}
		
		if(*pcmd == CE1612_FW_DATA_WP_CMD)
		{
			rc = ce1612_read(s_ctrl, &rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : uploader packet %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}
	sys_close(fd);
	SKYCDBG("%s : fw uploader data %d packets write OK !!!\n",__func__, i);

	msleep(5);

	rc = ce1612_poll(s_ctrl, 0xF5, 0x05, 10, 500);
	if (rc < 0)
	{
		SKYCERR("%s : uploader polling ERROR !!!\n",__func__);
		goto fw_update_fail;
	}
	/////////////////////////////////////////////////////////////////////////////////

	fd = sys_open(CE1612_FW_INFO_F, O_RDONLY, 0);
	if (fd < 0) {
		SKYCERR("%s: Can not open %s\n", __func__, CE1612_FW_INFO_F);
		goto fw_update_fail;
	}

	if (sys_read(fd, (char *)pdata, CE1612_NUM_CFG_CMD) != CE1612_NUM_CFG_CMD) {
		SKYCERR("%s: Can not read %s\n", __func__, CE1612_FW_INFO_F);
		sys_close(fd);
		goto fw_update_fail;
	}

	numPacket = (*(pdata+1) & 0xFF) << 8;
	numPacket |= *pdata & 0xFF;

	SKYCDBG("%s start : number of fw packets is 0x%x\n",__func__, numPacket);


	*pcmd= CE1612_FW_INFO_W_CMD;
	//	rc = ce1612_cmd(s_ctrl, cmd, pFW, CE1612_NUM_CFG_CMD);
	rc = msm_camera_i2c_txdata(s_ctrl->sensor_i2c_client, pFW, CE1612_NUM_CFG_CMD+1);
	if (rc < 0)
	{
		SKYCERR("%s : FW configs write ERROR 0x%x, 0x%x, 0x%x, 0x%x, 0x%x!!!\n",__func__, pFW[0], pFW[1], pFW[2], pFW[3], pFW[4]);
		sys_close(fd);
		goto fw_update_fail;
	}
	sys_close(fd);
	SKYCDBG("%s : fw info write OK !!!!\n",__func__);
	
	/////////////////////////////////////////////////////////////////////////////////

	fd = sys_open(CE1612_FW_BIN_F, O_RDONLY, 0);

	if (fd < 0) {
		SKYCERR("%s: Can not open %s\n", __func__, CE1612_FW_BIN_F);
		goto fw_update_fail;
	}

	for(i = 0; i < numPacket; i++)
	{
		if (sys_read(fd, (char *)pdata, CE1612_NUM_UPDATE_DATA) != CE1612_NUM_UPDATE_DATA) {
		SKYCERR("%s: Can not read %s : %d packet \n", __func__, CE1612_FW_BIN_F, i);
		sys_close(fd);
		goto fw_update_fail;
		}

		*pcmd= CE1612_FW_DATA_W_CMD;
		//	rc = ce1612_cmd(s_ctrl, cmd, pFW, CE1612_NUM_CFG_CMD);
		rc = msm_camera_i2c_txdata(s_ctrl->sensor_i2c_client, pFW, CE1612_NUM_UPDATE_DATA+1);
		if (rc < 0)
		{
			SKYCERR("%s : fw packet %d write ERROR !!!\n",__func__, i);
			sys_close(fd);
			goto fw_update_fail;
		}
		if(*pcmd == CE1612_FW_DATA_WP_CMD)
		{
			rc = ce1612_read(s_ctrl, &rdata, 1);
			if(rdata != 0)
			{
				SKYCERR("%s : fw packet %d write ERROR [0xF3 = 0x%x]!!!\n",__func__, i, rdata);
				goto fw_update_fail;
			}
		}
	}
	sys_close(fd);
	SKYCDBG("%s : fw data %d packets write OK !!!\n",__func__, i);
	/////////////////////////////////////////////////////////////////////////////////
	set_fs(old_fs);

	rc = ce1612_poll(s_ctrl, 0xF5, 0x06, 10, 3000);
	if (rc < 0)
	{
		SKYCERR("%s : fw data upgrade ERROR !!!\n",__func__);
		goto fw_update_fail;
	}

	return rc;	

fw_update_fail:
	set_fs(old_fs);
	SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);	
	return rc;
}


static int32_t ce1612_update_fw_boot(struct msm_sensor_ctrl_t *s_ctrl, const struct msm_camera_sensor_info *info)
{
	int32_t rc = 0;
	unsigned char data;
	unsigned char rdata[4];

#ifdef F_CE1612_POWER	
	rc = ce1612_sensor_power_up(s_ctrl);
	if (rc)
	{		
		SKYCERR(" ce1612_power failed rc=%d\n",rc);
		goto update_fw_boot_done; 
	}
#endif

	SKYCDBG("%s : Boot Start F0 for fw update !!\n", __func__);

	rc = ce1612_cmd(s_ctrl, 0xF0, NULL, 0);
	if (rc < 0)
	{
#if 1
		goto update_fw_boot_done;             
#else
		if(ce1612_saddr == 0x3C)
		ce1612_saddr = 0x3E;
		else
		ce1612_saddr = 0x3C;

		rc = ce1612_cmd(ce1612_saddr, 0xF0, NULL, 0);
		if (rc < 0)
		{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		goto update_fw_boot_done; 
		}
#endif
	}
	msleep(400);

	data = 0x00;
	rc = ce1612_cmd(s_ctrl, 0x00, &data, 1);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		//		goto update_fw_boot_done; 
		goto update_fw_boot; 
	}

	rc = ce1612_read(s_ctrl, rdata, 2);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		goto update_fw_boot; 
	}

	ce1612_ver.fw_minor_ver = rdata[0] & 0xFF;	  
	ce1612_ver.fw_major_ver = rdata[1] & 0xFF;
	SKYCDBG("%s : FW minor version=0x%x, FW major viersion=0x%x\n",__func__, ce1612_ver.fw_minor_ver, ce1612_ver.fw_major_ver);

	data = 0x01;
	rc = ce1612_cmd(s_ctrl, 0x00, &data, 1);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		//		goto update_fw_boot_done; 
		goto update_fw_boot; 
	}

	rc = ce1612_read(s_ctrl, rdata, 2);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		goto update_fw_boot; 
	}

	ce1612_ver.prm_minor_ver = rdata[0] & 0xFF;	  
	ce1612_ver.prm_major_ver = rdata[1] & 0xFF;  
	SKYCDBG("%s : jun PRM minor version=0x%x, PRM major viersion=0x%x\n",__func__, ce1612_ver.prm_minor_ver, ce1612_ver.prm_major_ver);

#if 0   // check AF ver.
	data = 0x05;
	rc = ce1612_cmd(s_ctrl, 0x00, &data, 1);
	if (rc < 0)
	{
	SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
	goto update_fw_boot; 
	}

	rc = ce1612_read(s_ctrl, rdata, 4);
	if (rc < 0)
	{
	SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
	goto update_fw_boot_done; 
	}
	SKYCDBG("%s : AF version[0]=0x%x, version[1]=0x%x, version[2]=0x%x, version[3]= 0x%x\n",__func__, rdata[0]&0xFF, rdata[1]&0xFF, rdata[2]&0xFF, rdata[3]&0xFF);
#endif

	if ((ce1612_ver.fw_major_ver == CE1612_FW_MAJOR_VER) && (ce1612_ver.fw_minor_ver == CE1612_FW_MINOR_VER)) {

		if ((ce1612_ver.prm_major_ver == CE1612_PRM_MAJOR_VER) &&
			(ce1612_ver.prm_minor_ver == CE1612_PRM_MINOR_VER)) {			
			SKYCDBG("%s : PRM minor version=0x%x, PRM major viersion=0x%x\n",__func__, CE1612_PRM_MINOR_VER, CE1612_PRM_MAJOR_VER);
			goto update_fw_boot_done;
		}		
	}


update_fw_boot: 
#ifdef F_CE1612_POWER	
	rc = ce1612_sensor_power_down(s_ctrl);
	if (rc) {
		SKYCERR(" ce1612_power failed rc=%d\n",rc);		
	}
	rc = ce1612_sensor_power_up(s_ctrl);
	if (rc) {		
		SKYCERR(" ce1612_power failed rc=%d\n",rc);
		goto update_fw_boot_done; 
	}
#endif
	rc = ce1612_update_fw(s_ctrl);

update_fw_boot_done:
#ifdef F_CE1612_POWER	
	SKYCDBG(" ce1612_sensor_release E\n");	
	rc = ce1612_sensor_power_down(s_ctrl);
	if (rc) {
		SKYCERR(" ce1612_power failed rc=%d\n",rc);		
	}
#endif

	return rc;
}
#endif


static int32_t ce1612_42_command(struct msm_sensor_ctrl_t *s_ctrl, uint8_t data)
{
    int32_t rc = 0;

	 SKYCDBG("%s: E\n",__func__);

    rc = ce1612_cmd(s_ctrl, 0x42, &data, 1);
    rc = ce1612_poll(s_ctrl, 0x43, data, 10, 100);
    if (rc < 0)
    {
		 SKYCERR("%s err(%d)\n", __func__, rc);
       return rc;
    }
   
    return rc;
}

static int32_t ce1612_lens_stop(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;

	SKYCDBG("%s: E\n",__func__);

	rc = ce1612_cmd(s_ctrl, 0x35, 0, 0);	// Lens Stop	
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}
	
	rc = ce1612_poll_bit(s_ctrl, 0x24, 10, 400);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}
	mdelay(10);

	SKYCDBG("%s: X\n",__func__);
	return rc;
}


static int32_t ce1612_set_coninuous_af(struct msm_sensor_ctrl_t *s_ctrl ,int8_t caf)
{

	uint8_t data_buf[4];	
	uint8_t read_data[2];
	int rc = 0;

	SKYCDBG("%s: caf = %d E\n",__func__, caf);

	continuous_af_mode = caf;    

	if(!(sensor_mode > 0 && sensor_mode < 4)){
		SKYCDBG("%s: Cancel sensor_mode= %d \n",__func__, sensor_mode);
		return 0;
	}
	rc = ce1612_lens_stop(s_ctrl);

	switch(caf)
	{
		case 1:     //AF_MODE_CONTINUOUS (AF-C)
			rc = ce1612_cmd_read(s_ctrl, 0x2D, read_data, 2);	// check AF-T
			if(read_data[0] == 1)
			{
				data_buf[0] = 0x00;
				rc = ce1612_cmd(s_ctrl, 0x2C, data_buf, 1);
			}
			SKYCDBG("AF-C start\n");
			
			data_buf[0] = 0x02;
			rc = ce1612_cmd(s_ctrl, 0x20, data_buf, 1);
			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}	
			
			rc = ce1612_poll_bit(s_ctrl, 0x24, 10, 400);
			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}		

			rc = ce1612_cmd(s_ctrl, 0x23, 0, 0);
			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}
			break;

		case 2 :    //AF_MODE_CAF (AF-T)
			SKYCDBG("AF-T start\n");
			data_buf[0] = 0x00;
			rc = ce1612_cmd(s_ctrl, 0x20, data_buf, 1); //set focus mode normal
			rc = ce1612_poll_bit(s_ctrl, 0x24, 10, 400);

			data_buf[0] = 0x01;
			rc = ce1612_cmd(s_ctrl, 0x2C, data_buf, 1);
			//rc = ce1612_poll_bit(s_ctrl, 0x24, 10, 400);
			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}	
			break;

		default :   // continuous AF OFF    
			rc = ce1612_cmd_read(s_ctrl, 0x2D, read_data, 2);	// check AF-T
			if(read_data[0] == 1)
			{
				SKYCDBG("Continuous AF OFF\n");
				data_buf[0] = 0x00;
				rc = ce1612_cmd(s_ctrl, 0x2C, data_buf, 1);
				rc = ce1612_poll_bit(s_ctrl, 0x24, 10, 400);
				if (rc < 0)
				{
					SKYCERR("%s err(%d)\n", __func__, rc);
					return rc;
				}	
			}
			break;
	}

	SKYCDBG("%s: X\n",__func__);
	return rc;
}


static int32_t ce1612_lens_stability(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	uint8_t rdata = 0;
	uint8_t data_buf[4];

	SKYCDBG("%s: sensor_mode(%d) E\n",__func__, sensor_mode);

	if(sensor_mode == -1)
		 return rc;

	rc = ce1612_cmd_read(s_ctrl, 0x6C, &rdata, 1);

	if((rdata > 0) && (rdata < 8))
	{
		return 0;
	}

	ce1612_set_coninuous_af(s_ctrl, 0); // AF-T stop	
	data_buf[0] = 0x00;
	rc = ce1612_cmd(s_ctrl, 0x20, data_buf, 1); //set focus mode normal
	rc = ce1612_poll_bit(s_ctrl, 0x24, 10, 400);

	SKYCDBG("%s: X\n",__func__);
	return rc;
}


static int32_t ce1612_set_focus_rect_internel(struct msm_sensor_ctrl_t *s_ctrl)
{
    uint8_t data_buf[10];
    int32_t is_af_t = 0;

    int32_t rc = 0;
    //uint8_t read_data =0;

    SKYCDBG("%s E\n",__func__);

    if(!(sensor_mode > 0 && sensor_mode < 4))
        goto set_rect_end;

    if(!(x1|y1|x2|y2)) {
        goto set_rect_end;
    }
#if 0
    rc = ce1612_cmd_read(s_ctrl, 0x24, &read_data, 1);
	SKYCDBG("%s read_data(%d)\n",__func__, read_data);
	 
    if (rc < 0)
    {
		 SKYCERR("%s err(%d)\n", __func__, rc);
		 return rc;
    }

    if(read_data & 0x01)
#endif	 	
    {
#if 1 // AF-T state check
    	rc = ce1612_cmd_read(s_ctrl, 0x2D, data_buf, 2);
    	if (rc < 0)
    	{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
    	}

    	if(data_buf[0] == 0x01)
    	{
	      is_af_t  = 1;
	      data_buf[0] = 0x02;
	      rc = ce1612_cmd(s_ctrl, 0x2C, data_buf, 1);
	      if (rc < 0)
	      {
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
	      }	
    	}		
#endif

		rc = ce1612_lens_stop(s_ctrl);

#if 1 // AF-T state check
    	if(is_af_t  == 1)
    	{
			data_buf[0] = 0x01;
			rc = ce1612_cmd(s_ctrl, 0x2C, data_buf, 1);
			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}	
    	}		
#endif	
    }

    SKYCDBG("%s  xleft = %d, ytop = %d, xright = %d, ybottom = %d\n",__func__, x1, y1, x2, y2);

    data_buf[0] = 0x05;
    data_buf[1] = 0x03;
    data_buf[2] = x1 & 0xff;
    data_buf[3] = (x1 >> 8) & 0xff;
    data_buf[4] = y1 & 0xff;
    data_buf[5] = (y1 >> 8) & 0xff;
    data_buf[6] = x2 & 0xff;
    data_buf[7] = (x2 >> 8) & 0xff;
    data_buf[8] = y2 & 0xff;
    data_buf[9] = (y2 >> 8) & 0xff;
	
    rc = ce1612_cmd(s_ctrl, 0x41, data_buf, 10);
    if (rc < 0)
    {
		 SKYCERR("%s err(%d)\n", __func__, rc);
		 return rc;
    }	

    ce1612_42_command(s_ctrl, 0x05);	 

set_rect_end:
    
#if 0 //def CONFIG_PANTECH_CAMERA//IRQ // AF-T state check
    rc = ce1612_cmd_read(0x2D, data_buf, 2);
    if (rc < 0)
    {
        SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
        return rc;
    }

    if(data_buf[0] == 0x01)
    {
        ce1612_irq_stat = 1;	// first trigger start
    }		
#endif

	 SKYCDBG("%s: X\n",__func__);
    return rc;	
}


static int32_t ce1612_set_focus_rect(struct msm_sensor_ctrl_t *s_ctrl, int32_t focus_rect)
{
    int32_t focus_rect_size = 256;
    int32_t focus_rect_size_1 = 255;
    int32_t focus_rect_size_half_1 = 127;

    //uint8_t data_buf[10];
    int32_t x_c, y_c, xleft, xright, ytop, ybottom;
    int32_t width, height;

    int32_t rc = 0;

	 SKYCDBG("%s: sensor_mode(%d) E\n",__func__, sensor_mode);


    if(!(sensor_mode > 0 && sensor_mode < 4))
        return rc;

    if (focus_rect == 0) {
        ce1612_42_command(s_ctrl, 0x00);  
        return rc;
    }

    width = ce1612_dimensions[sensor_mode].x_output;
    height = ce1612_dimensions[sensor_mode].y_output;

#if 0
    if(height == CE1612_ZSL_SIZE_HEIGHT)
    {
        focus_rect_size = 256;
        focus_rect_size_1 = 255;
        focus_rect_size_half_1 = 127;
    }
#endif

    x_c = (int32_t)((focus_rect & 0xffff0000) >> 16);
    x_c = (x_c*width)/2000;
    y_c = (int32_t)(focus_rect & 0xffff);
    y_c = (y_c*height)/2000;

    xleft = x_c - focus_rect_size_half_1;
    if(xleft < 0)
        xleft = 0;
    if(xleft > width - focus_rect_size)
        xleft = width - focus_rect_size;

    ytop = y_c - focus_rect_size_half_1;
    if(ytop < 0)
        ytop = 0;
    if(ytop > height - focus_rect_size)
        ytop = height - focus_rect_size;

    xright = xleft + focus_rect_size_1;
    ybottom = ytop + focus_rect_size_1;

    SKYCDBG("%s  xleft = %d, ytop = %d, xright = %d, ybottom = %d\n",__func__, xleft, ytop, xright, ybottom);

    x1 = xleft;
    x2 = xright;
    y1 = ytop;
    y2 = ybottom;

	 SKYCDBG("%s: continuous_af_mode(%d) E\n",__func__, continuous_af_mode);	 

#if 1 // AF-T state check
    if(continuous_af_mode == 0)
#else
    rc = ce1612_cmd_read(s_ctrl, 0x2D, data_buf, 2);
    if (rc < 0)
    {
        SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
        return rc;
    }

    if(data_buf[0] == 0x01)
    {
        return rc;
    }		
#endif

    rc = ce1612_set_focus_rect_internel(s_ctrl);

	 SKYCDBG("%s: X\n",__func__);
    return rc;	
}


static int32_t ce1612_sensor_set_auto_focus(struct msm_sensor_ctrl_t *s_ctrl, int8_t autofocus)
{
    int32_t rc = 0;
    uint8_t read_data =0;
    uint8_t data_buf[10];

    if(!(sensor_mode > 0 && sensor_mode < 4)) // test
        return 0;

    SKYCDBG("%s  auto_focus = %d\n",__func__, autofocus);
	 
    if ((autofocus < 0) || (autofocus > 6))
    {
		  SKYCERR("%s autofocus(%d)\n", __func__, autofocus);	 
        return 0;//-EINVAL;
    }
	 
    if(autofocus == 6)  //cancel AF
    {
        rc = ce1612_lens_stop(s_ctrl);
        return rc;
    }
    
#if 1 // AF-T state check
    if(continuous_af_mode != 0)
    {
        rc = ce1612_set_focus_rect_internel(s_ctrl);    
        return rc;
    }
#endif

	SKYCDBG("%s autofocus mode(%d)\n",__func__, autofocus);

#if 0
	rc = ce1612_cmd_read(s_ctrl, 0x24, &read_data, 1);
	SKYCDBG("%s read_data(%d)\n",__func__, read_data);

	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	if(read_data & 0x01)
#endif		
	{
		rc = ce1612_lens_stop(s_ctrl);
	}

    rc = ce1612_cmd_read(s_ctrl, 0x43, &read_data, 1);
	 SKYCDBG("%s read_data(%d)\n",__func__, read_data);

    if(read_data == 0x05){
        goto start_af;
    }

    switch(autofocus)
    {
    case 1:	// MACRO
		  ce1612_42_command(s_ctrl, 0x00);  

        data_buf[0] = 0x01;
        rc = ce1612_cmd(s_ctrl, 0x20, data_buf, 1);
        rc = ce1612_poll_bit(s_ctrl, 0x24, 10, 400);
        if (rc < 0)
        {
			   SKYCERR("%s err(%d)\n", __func__, rc);
            return rc;
        }
        break;

//    default:	// NORMAL
//    case 2:	// AUTO
    case 0:	// NORMAL
		  ce1612_42_command(s_ctrl, 0x00);  

        data_buf[0] = 0x00;
        rc = ce1612_cmd(s_ctrl, 0x20, data_buf, 1);        
        rc = ce1612_poll_bit(s_ctrl, 0x24, 10, 400);
        if (rc < 0)
        {
			   SKYCERR("%s err(%d)\n", __func__, rc);
            return rc;
        }		
        break;
		  
    default:
        return 0;
        break;
    }
    
start_af:
    rc = ce1612_cmd(s_ctrl, 0x23, 0, 0);

	 SKYCDBG("%s: X\n",__func__);
    return rc;
}


static int32_t ce1612_sensor_check_af(struct msm_sensor_ctrl_t *s_ctrl ,int8_t autofocus)
{
    uint8_t rdata = 0;
    uint8_t data_buf[4];
    int32_t rc = 0;

#if 0        
    SKYCDBG("%s: sensor_mode=%d E\n",__func__, sensor_mode);
#endif

    if(!(sensor_mode > 0 && sensor_mode < 4))
        return 0;

	if(continuous_af_mode == 1) 
		return 0;

    rc = ce1612_cmd_read(s_ctrl, 0x24, &rdata, 1);
#if 0        
    SKYCDBG("%s: rdata(%d) X\n",__func__, rdata);
#endif
    if (rc < 0){
        SKYCERR("%s err(%d)\n", __func__, rc);	 	
        return rc;
    }
    
    if (!(rdata & 0x01))
    {
        rc = 0;
#if 1
        data_buf[0] = 0x00;
        rc = ce1612_cmd(s_ctrl, 0x42, data_buf, 1);
#endif
    }
    else
        rc = -1;

    SKYCDBG("%s: rdata(%d) X\n",__func__, rdata);
    return rc;
}

void ce1612_sensor_start_stream(struct msm_sensor_ctrl_t *s_ctrl)
{

#if 1 
SKYCDBG("%s: %d\n", __func__, __LINE__);
#endif

}

static int32_t ce1612_ZSL_config(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	uint8_t data_buf[4];
	uint8_t rdata = 0;

	rc = ce1612_cmd_read(s_ctrl, 0x6C, &rdata, 1);
	SKYCDBG("%s: rdata=%d, sensor_mode=%d E\n",__func__, rdata, sensor_mode);

	if((rdata > 0) && (rdata < 8))
	{
		// stop Capture 
		rc = ce1612_cmd(s_ctrl, 0x75, 0, 0);
		if (rc < 0)
		{
			SKYCERR("ERR:%s Capture Stop command FAIL!!!rc=%d return~~\n", __func__, rc);
			return rc;
		}	

		rc = ce1612_poll(s_ctrl, 0x6C, 0x00, 10, 100);
		if (rc < 0)
		{
			SKYCERR("%s : Capture Stop polling ERROR !!!\n",__func__);
			return rc;
		}		
	}
	else if(rdata == 8)
	{
		goto preview_ok;
	}
	else if(rdata == 9)
	{
		rc = ce1612_poll(s_ctrl, 0x6C, 0x08, 10, 100);
		if (rc < 0)
		{
			SKYCERR("%s : Preview Start polling ERROR !!!\n",__func__);
			return rc;
		}	
		goto preview_ok;
	}

	SKYCDBG("%s : ZSL Preview Start, zsl_status=%d !!!\n",__func__, zsl_status);


//----------------------------------------------------//

	// AE/AWB enable	
	data_buf[0] = 0x01;
	data_buf[1] = 0x10;
	rc = ce1612_cmd(s_ctrl, 0x04, data_buf, 2);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}	

	rc = ce1612_cmd(s_ctrl, 0x01, NULL, 0);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}	

//----------------------------------------------------//

	data_buf[0] = 0x00;
	rc = ce1612_cmd(s_ctrl, 0x5B, data_buf, 1);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}	

//----------------------------------------------------//

	data_buf[0] = 0x00;
	rc = ce1612_cmd(s_ctrl, 0x11, data_buf, 1);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}	

//----------------------------------------------------/

	data_buf[0] = 0x20; 
	data_buf[1] = 0x00;
	data_buf[2] = 0x00;
	rc = ce1612_cmd(s_ctrl, 0x54, data_buf, 3);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

//----------------------------------------------------/

	data_buf[0] = 0x20; 
	data_buf[1] = 0x00;
	data_buf[2] = 0x00;
	data_buf[3] = 0x00;
	rc = ce1612_cmd(s_ctrl, 0x73, data_buf, 4);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}	

//----------------------------------------------------/

	// ZSL Preview start (PREVIEW)
	data_buf[0] = 0x01; 
	rc = ce1612_cmd(s_ctrl, 0x6B, data_buf, 1);

	rc = ce1612_poll(s_ctrl, 0x6C, 0x08, 10, 100);  
	if (rc < 0)
	{
		SKYCERR("%s : ZSL Preview Start polling ERROR !!!\n",__func__);
		return rc;
	} 

preview_ok:
	mdelay(30); // test 04.13.

	zsl_status = sensor_mode = 3;
	ce1612_set_coninuous_af(s_ctrl, continuous_af_mode);

	//	f_stop_capture = 0;	// test cts
	x1 = 0;
	x2 = 0;
	y1 = 0;
	y2 = 0;

	SKYCDBG("%s end rc = %d\n",__func__, rc);
	return rc;
}


static int32_t ce1612_video_config(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	uint8_t data_buf[4];
	uint8_t rdata = 0;

	rc = ce1612_cmd_read(s_ctrl, 0x6C, &rdata, 1);
	SKYCDBG("%s: rdata=%d, sensor_mode=%d E\n",__func__, rdata, sensor_mode);


	if((rdata > 0) && (rdata < 8))
	{
		// stop Capture	
		rc = ce1612_cmd(s_ctrl, 0x75, 0, 0);
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}	

		rc = ce1612_poll(s_ctrl, 0x6C, 0x00, 50, 100);
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}		
	}
	else if(rdata == 8)
	{
		goto preview_ok;
	}
	else if(rdata == 9)
	{
		rc = ce1612_poll(s_ctrl, 0x6C, 0x08, 50, 100);
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}	
		goto preview_ok;
	}

	SKYCDBG("%s : Preview Start, zsl_status=%d !!!\n",__func__, zsl_status);

//----------------------------------------------------//

	if(zsl_status == 3){
		
		data_buf[0] = 0x01;
		data_buf[1] = 0x00;
		rc = ce1612_cmd(s_ctrl, 0x04, data_buf, 2);
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}	

		rc = ce1612_cmd(s_ctrl, 0x01, NULL, 0);
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}	
		
		// Capture Size (YUV)
		data_buf[0] = 0x21;
		data_buf[1] = 0x05;
		data_buf[2] = 0x00; 
		data_buf[3] = 0x00; 
		
		rc = ce1612_cmd(s_ctrl, 0x73, data_buf, 4);
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}

		zsl_status = 1;		
	}   
//----------------------------------------------------/

	// Preview Size
	data_buf[0] = 0x1C;	//SXGA(1280x960)
	data_buf[1] = 0x01;
	data_buf[2] = 0x00;	
	rc = ce1612_cmd(s_ctrl, 0x54, data_buf, 3);

	// AE/AWB enable
	data_buf[0] = 0x00;
	rc = ce1612_cmd(s_ctrl, 0x11, data_buf, 1);

	// Preview start (PREVIEW)
	data_buf[0] = 0x01;
	rc = ce1612_cmd(s_ctrl, 0x6B, data_buf, 1);

	rc = ce1612_poll(s_ctrl, 0x6C, 0x08, 50, 100);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

preview_ok:
	sensor_mode = 1;
	ce1612_set_coninuous_af(s_ctrl, continuous_af_mode);

	//	f_stop_capture = 0;	// test cts
	x1 = 0;
	x2 = 0;
	y1 = 0;
	y2 = 0;

	SKYCDBG("%s: X\n",__func__);
	return rc;
}


static int32_t ce1612_snapshot_config(struct msm_sensor_ctrl_t *s_ctrl)
{
    int32_t rc = 0;
    uint8_t data_buf[10];

    int8_t prev_caf = continuous_af_mode;
    rc = ce1612_set_coninuous_af(s_ctrl, 0);
    continuous_af_mode = prev_caf;
    rc = ce1612_lens_stop(s_ctrl);

	 SKYCDBG("%s: E\n",__func__);

    // additional setting
    data_buf[0] = 0x00;
    rc = ce1612_cmd(s_ctrl, 0x65, data_buf, 1);
    if (rc < 0)
    {
		 SKYCERR("%s err(%d)\n", __func__, rc);
		 return rc;
    }

    rc = ce1612_cmd(s_ctrl, 0x74, 0, 0);
    if (rc < 0)
    {
		 SKYCERR("%s err(%d)\n", __func__, rc);
		 return rc;
    }

    // AE/AWB enable
    data_buf[0] = 0x11;
    rc = ce1612_cmd(s_ctrl, 0x11, data_buf, 1);

    // Buffering Capture start
    rc = ce1612_cmd(s_ctrl, 0x74, 0, 0);
	 
	 SKYCDBG("%s: X\n",__func__);
    return rc;
}


void ce1612_sensor_stop_stream(struct msm_sensor_ctrl_t *s_ctrl)
{
    int rc = 0;
    uint8_t data_buf[2];

    SKYCDBG("%s: sensor_mode = %d\n", __func__, sensor_mode);

	// 	if(!(sensor_mode > 0 && sensor_mode < 4))
	if((sensor_mode < 0) || (sensor_mode > 3))
		return;

	if(sensor_mode != 0)	 
		rc = ce1612_lens_stop(s_ctrl);

	if(sensor_mode == 0)
	{
		// stop Capture 
		rc = ce1612_cmd(s_ctrl, 0x75, 0, 0);
		if (rc < 0)
		{
			SKYCERR("ERR:%s Capture Stop command FAIL!!!rc=%d return~~\n", __func__, rc);
			return;
		}	

		rc = ce1612_poll(s_ctrl, 0x6C, 0x00, 10, 100);
		if (rc < 0)
		{
			SKYCERR("%s : Capture Stop polling ERROR !!!\n",__func__);
			return;
		}			  
	}
	else
	{
		data_buf[0] = 0x00;	
		rc = ce1612_cmd(s_ctrl, 0x6B, data_buf, 1);
		rc = ce1612_poll(s_ctrl, 0x6C, 0x00, 10, 100);
		if (rc < 0)
		{
			SKYCERR("%s : Preview Stop polling ERROR !!!\n",__func__);
			return;
		}	
	}

    sensor_mode = -1;
    SKYCDBG("%s: END -(%d) \n", __func__, rc);	
}


int ce1612_sensor_init(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc = 0;
	uint8_t data_buf[4];

	SKYCDBG("%s: E\n",__func__);

	// ISP FW Boot UP !!
	rc = ce1612_cmd(&ce1612_s_ctrl, 0xF0, NULL, 0);
	mdelay(70);

	// Preview Stop
	//ce1612_sensor_stop_stream(s_ctrl); //charley2

	// Preview Size
	data_buf[0] = 0x1C;	//SXGA(1280x960)
	data_buf[1] = 0x01;
	data_buf[2] = 0x00; 
	rc = ce1612_cmd(s_ctrl, 0x54, data_buf, 3);

	// Capture Size (YUV)
	data_buf[0] = 0x21;
	data_buf[1] = 0x05;
	data_buf[2] = 0x00; 
	data_buf[3] = 0x00; 

	rc = ce1612_cmd(s_ctrl, 0x73, data_buf, 4);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	sensor_mode = 1;

	SKYCDBG("%s: X\n",__func__);	
	return rc;	
}

int32_t ce1612_sensor_setting(struct msm_sensor_ctrl_t *s_ctrl, int update_type, int res)
{
	int32_t rc = 0;

	SKYCDBG("%s: update_type = %d, res=%d E\n", __func__, update_type, res);

#if 1
	/*   if (s_ctrl->func_tbl->sensor_stop_stream)
	s_ctrl->func_tbl->sensor_stop_stream(s_ctrl);
	*/
	if(res != 0)
		ce1612_sensor_stop_stream(s_ctrl);
#endif
	msleep(30);
	if (update_type == MSM_SENSOR_REG_INIT) {
		s_ctrl->curr_csi_params = NULL;
		msm_sensor_enable_debugfs(s_ctrl);
		ce1612_sensor_init(s_ctrl);
		
	} else if (update_type == MSM_SENSOR_UPDATE_PERIODIC) {
	
		// msm_sensor_write_res_settings(s_ctrl, res);
		if (s_ctrl->curr_csi_params != s_ctrl->csi_params[res]) {
		
			SKYCDBG("%s: ==> MIPI setting  E %d\n", __func__, update_type);
			s_ctrl->curr_csi_params = s_ctrl->csi_params[res];

			s_ctrl->curr_csi_params->csid_params.lane_assign =
				s_ctrl->sensordata->sensor_platform_info->
				csi_lane_params->csi_lane_assign;
			s_ctrl->curr_csi_params->csiphy_params.lane_mask =
				s_ctrl->sensordata->sensor_platform_info->
				csi_lane_params->csi_lane_mask;

			v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
				NOTIFY_CSID_CFG,
				&s_ctrl->curr_csi_params->csid_params);
 
			mb();
			v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
				NOTIFY_CSIPHY_CFG,
				&s_ctrl->curr_csi_params->csiphy_params);
			mb();
			msleep(20);
			SKYCDBG("%s: ==> MIPI setting  X %d\n", __func__, update_type);			
		}

		v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
			NOTIFY_PCLK_CHANGE, &s_ctrl->msm_sensor_reg->
			output_settings[res].op_pixel_clk); 
			
		switch (res) {
			case 0:
				rc = ce1612_snapshot_config(s_ctrl);	
				break;

			case 1:
				rc = ce1612_video_config(s_ctrl);
				SKYCDBG("Sensor setting : Case 1 Video config"); 
				break;

	      //case 2: 
			//	rc = ce1612_1080p_config(s_ctrl);	
			//	break;

			case 3: 
				rc = ce1612_ZSL_config(s_ctrl);	
				SKYCDBG("Sensor setting : Case 3 ZSL config");				
				break;	
				 
			default:
				rc = ce1612_video_config(s_ctrl);
				SKYCDBG("Sensor setting : Default Video config"); 
				break;
		}
		sensor_mode = res;
		SKYCDBG("Sensor setting : Res = %d\n", res);

		//msleep(30);
	}
	SKYCDBG("%s: %d x\n", __func__, __LINE__);
	return rc;
}


#ifdef CONFIG_PANTECH_CAMERA
static int ce1612_sensor_set_brightness(struct msm_sensor_ctrl_t *s_ctrl ,int8_t brightness)
{
	uint8_t data_buf[2];
	int rc = 0;

	SKYCDBG("%s: %d E\n",__func__, brightness);

	if(sensor_mode == 0) // test
		return 0;

	if(brightness < 0 || brightness >= CE1612_BRIGHTNESS_MAX){
		SKYCERR("%s err(-EINVAL)\n", __func__);
		return -EINVAL;
	}

	data_buf[0] = 0x02;
	data_buf[1] = ce1612_bright_data[brightness];
	rc = ce1612_cmd(s_ctrl, 0x04, data_buf, 2);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}	

	//Refesh
	rc = ce1612_cmd(s_ctrl, 0x01, NULL, 0);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}	

	SKYCDBG("%s: X\n",__func__);
	return rc;
}


static int ce1612_sensor_set_effect(struct msm_sensor_ctrl_t *s_ctrl ,int8_t effect)
{
	uint8_t data_buf[2];
	int rc = 0;

	SKYCDBG("%s: %d E\n",__func__, effect);

	if(sensor_mode == 0) // test
		return 0;

	if(effect < 0 || effect >= CE1612_EFFECT_MAX){
		SKYCERR("%s err(-EINVAL)\n", __func__);
		return -EINVAL;
	}

	if(effect == 5) //posterize
	{
		data_buf[0] = 0x05;
		data_buf[1] = ce1612_effect_data[effect];
		rc = ce1612_cmd(s_ctrl, 0x3D, data_buf, 2);	//effect off
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}

		data_buf[0] = 0x07;
		data_buf[1] = 0x0E;
		rc = ce1612_cmd(s_ctrl, 0x3D, data_buf, 2);
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}	
	}
	else
	{
		data_buf[0] = 0x05;
		data_buf[1] = ce1612_effect_data[effect];
		rc = ce1612_cmd(s_ctrl, 0x3D, data_buf, 2);
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}	

		data_buf[0] = 0x07;
		data_buf[1] = 0x06;
		rc = ce1612_cmd(s_ctrl, 0x3D, data_buf, 2);	// gamma default		
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}	
	}

   //Refresh
	rc = ce1612_cmd(s_ctrl, 0x01, NULL, 0);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}	

	SKYCDBG("%s: X\n",__func__);
	return rc;
}


static int ce1612_sensor_set_exposure_mode(struct msm_sensor_ctrl_t *s_ctrl ,int8_t exposure)
{
	uint8_t data_buf[2];
	int32_t rc = 0;

	SKYCDBG("%s: %d E\n",__func__, exposure);

	if(sensor_mode == 0) // test
		return 0;

	if ((exposure < 0) || (exposure >= CE1612_EXPOSURE_MAX))
	{
		SKYCERR("%s err(-EINVAL)\n", __func__);
		return -EINVAL;
	}

	data_buf[0] = 0x00;
	data_buf[1] = ce1612_exposure_data[exposure];
	
	rc = ce1612_cmd(s_ctrl, 0x04, data_buf, 2);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}	

	//Refresh
	rc = ce1612_cmd(s_ctrl, 0x01, NULL, 0);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}	

	SKYCDBG("%s: X\n",__func__);
	return rc;
}


static int ce1612_sensor_set_wb(struct msm_sensor_ctrl_t *s_ctrl, int8_t wb)
{
	uint8_t data_buf[2];
	int rc = 0;

	SKYCDBG("%s: %d E\n",__func__, wb);

	if(sensor_mode == 0) // test
		return 0;

	if(wb == 9)
		wb = 1;

	if(wb < 1 || wb > CE1612_WHITEBALANCE_MAX){
		SKYCERR("%s err(-EINVAL)\n", __func__);
		return -EINVAL;
	}

	if(wb == 1 || wb == 2) // auto
	{
		data_buf[0] = 0x00;
		rc = ce1612_cmd(s_ctrl, 0x1A, data_buf, 1);
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}	

		data_buf[0] = 0x11;
		data_buf[1] = 0x00;
		
		rc = ce1612_cmd(s_ctrl, 0x04, data_buf, 2);	
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}	
	}	
	else
	{
		data_buf[0] = 0x01;
		rc = ce1612_cmd(s_ctrl, 0x1A, data_buf, 1);
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}	

		data_buf[0] = 0x10;
		data_buf[1] = ce1612_wb_data[wb-1];
		
		rc = ce1612_cmd(s_ctrl, 0x04, data_buf, 2);	
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}	
	}	

   //Refresh
	rc = ce1612_cmd(s_ctrl, 0x01, NULL, 0);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}	

	SKYCDBG("%s: X\n",__func__);
	return rc;
}


static int32_t ce1612_set_scene_mode(struct msm_sensor_ctrl_t *s_ctrl, int8_t scene_mode)
{
	uint8_t data_buf[2];
	int rc = 0;

	SKYCDBG("%s: %d E\n",__func__, scene_mode);

	if(sensor_mode == 0) // test
		return 0;    

	switch (scene_mode)
	{
		case 0: //OFF
			data_buf[0] = 0x00;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(s_ctrl, 0x82, data_buf, 2);
			
			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}


			data_buf[0] = 0x00;
			rc = ce1612_cmd(s_ctrl, 0x1A, data_buf, 1);

			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}
			break;

		case 7: //Potrait
			data_buf[0] = 0x02;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(s_ctrl, 0x82, data_buf, 2);

			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}	
			break;

		case 2: //LandScape
			data_buf[0] = 0x02;
			data_buf[1] = 0x01;
			rc = ce1612_cmd(s_ctrl, 0x82, data_buf, 2);

			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}
			break;

		case 19: //Indoor
			data_buf[0] = 0x02;
			data_buf[1] = 0x06;
			rc = ce1612_cmd(s_ctrl, 0x82, data_buf, 2);

			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}
			break;

		case 14: //Party
			data_buf[0] = 0x02;
			data_buf[1] = 0x06;
			rc = ce1612_cmd(s_ctrl, 0x82, data_buf, 2);

			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}
			break;

		case 9: //Sports
			data_buf[0] = 0x02;
			data_buf[1] = 0x05;
			rc = ce1612_cmd(s_ctrl, 0x82, data_buf, 2);

			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}
			break;

		case 6: //Night
			data_buf[0] = 0x02;
			data_buf[1] = 0x03;
			rc = ce1612_cmd(s_ctrl, 0x82, data_buf, 2);

			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}
			break;
		
		case 3: //Snow
		case 4: //Beach
			data_buf[0] = 0x02;
			data_buf[1] = 0x07;
			rc = ce1612_cmd(s_ctrl, 0x82, data_buf, 2);

			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}
			break;

		case 5: //Sunset
			data_buf[0] = 0x02;
			data_buf[1] = 0x02;
			rc = ce1612_cmd(s_ctrl, 0x82, data_buf, 2);

			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}
			break;

		case 20: //TEXT
			data_buf[0] = 0x02;
			data_buf[1] = 0x04;
			rc = ce1612_cmd(s_ctrl, 0x82, data_buf, 2);

			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}
			break;

		default:
			data_buf[0] = 0x00;
			rc = ce1612_cmd(s_ctrl, 0x1A, data_buf, 1);

			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}
			break;
	}

   //Refresh
	rc = ce1612_cmd(s_ctrl, 0x01, NULL, 0);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	SKYCDBG("%s: X\n",__func__);
	return rc;
}


static int ce1612_sensor_set_preview_fps(struct msm_sensor_ctrl_t *s_ctrl ,int8_t preview_fps)
{
	/* 0 : variable 30fps, 1 ~ 30 : fixed fps */
	/* default: variable 8 ~ 30fps */
	
	uint8_t data_buf[4];
	int32_t rc = 0;

	SKYCDBG("%s: %d E\n",__func__, preview_fps);

	if ((preview_fps < C_PANTECH_CAMERA_MIN_PREVIEW_FPS) || (preview_fps > C_PANTECH_CAMERA_MAX_PREVIEW_FPS)) {
		SKYCERR("%s err(-EINVAL)\n", __func__);
		return -EINVAL;
	}

	if(preview_fps == C_PANTECH_CAMERA_MAX_PREVIEW_FPS)
	{
		data_buf[0] = 0x01;
		data_buf[1] = 0xFF;
		data_buf[2] = 0xFF;
		data_buf[3] = 0x00;

		rc = ce1612_cmd(s_ctrl, 0x5A, data_buf, 4);
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}		
	}
	else
	{

		if (preview_fps== 30){
			data_buf[0] = 0x01;
			data_buf[1] = 0x14;
			rc = ce1612_cmd(s_ctrl, 0x04, data_buf, 2);
			if (rc < 0)
			{
				SKYCERR("%s : Preview 30fps ERROR !!!, rc = %d\n",__func__, rc);
				return rc;
			}

			rc = ce1612_cmd(s_ctrl, 0x01, 0, 0);
			if (rc < 0)
			{
				SKYCERR("%s : Preview 30fps set ERROR !!!, rc = %d\n",__func__, rc);
				return rc;
			}
		}else{	
			data_buf[0] = 0x01;
			data_buf[1] = preview_fps & 0xFF;
			data_buf[2] = (preview_fps >> 8) & 0xFF;
			data_buf[3] = 0x00;

			rc = ce1612_cmd(s_ctrl, 0x5A, data_buf, 4);
			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}	
		}	
	}
	
	SKYCDBG("%s: X\n",__func__);
	return rc;
}


static int ce1612_sensor_set_reflect(struct msm_sensor_ctrl_t *s_ctrl ,int8_t reflect)
{
	int rc = 0;
	return rc;
}


static int32_t ce1612_set_antibanding(struct msm_sensor_ctrl_t *s_ctrl, int8_t antibanding)
{
	uint8_t data_buf[2];
	int32_t rc = 0;

	SKYCDBG("%s: %d E\n",__func__, antibanding);

	if ((antibanding < 0) || (antibanding >= CE1612_FLICKER_MAX))
	{
		SKYCERR("%s err(-EINVAL)\n", __func__);
		return -EINVAL;
	}

	switch(antibanding)
	{
		case 1 : //60Hz
			data_buf[0] = 0x03;
			rc = ce1612_cmd(s_ctrl, 0x14, data_buf, 1);

			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}
			break;

		case 2 : //50Hz
			data_buf[0] = 0x02;
			rc = ce1612_cmd(s_ctrl, 0x14, data_buf, 1);

			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}
			break;

		case 0 : 
		case 3 : //Auto
			data_buf[0] = 0x01;
			rc = ce1612_cmd(s_ctrl, 0x14, data_buf, 1);

			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}
			break;		

		default: 
			SKYCERR("%s: antibanding(%d)\n", __func__, antibanding);
			break;
	}
	
	SKYCDBG("%s: X\n",__func__);
	return rc;
}

static int32_t ce1612_set_antishake(struct msm_sensor_ctrl_t *s_ctrl ,int8_t antishake)
{
	uint8_t data_buf[2];
	int32_t rc = 0;


	SKYCDBG("%s: %d E\n",__func__, antishake);

	if(sensor_mode == 0) // test
		return 0;    

	switch(antishake)
	{
		case 0 :
		case 1 :
			data_buf[0] = antishake;
			
			rc = ce1612_cmd(s_ctrl, 0x5B, data_buf, 1);
			if (rc < 0)
			{
				SKYCERR("%s err(%d)\n", __func__, rc);
				return rc;
			}	
			break;

		default :
			SKYCERR("%s: antishake(%d)\n", __func__, antishake);
			break;        
	}

	SKYCDBG("%s: X\n",__func__);
	return rc;
}

static int32_t ce1612_set_wdr(struct msm_sensor_ctrl_t *s_ctrl, int8_t wdr)
{
	uint8_t data_buf[2];
	int32_t rc = 0;

	SKYCDBG("%s: wdr = %d E\n",__func__, wdr);

	if ((wdr != 0) && (wdr != 1))
	{
		SKYCERR("%s FAIL!!! return~~  wdr = %d\n",__func__, wdr);
		return 0;//-EINVAL;
	}

	switch(wdr)
	{
		case 0 :
			data_buf[0] = 0x00;	
	rc = ce1612_cmd(s_ctrl, 0x88, data_buf, 1);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	
			break;

		case 1 :
			data_buf[0] = 0x01;	
	rc = ce1612_cmd(s_ctrl, 0x88, data_buf, 1);
	if (rc < 0)
	{
		SKYCERR("ERR:%s FAIL!!!rc=%d return~~\n", __func__, rc);
		return rc;
	}	
			break;

		default :
			SKYCERR("%s: Default wdr(%d)\n", __func__, wdr);
			break;        
	}

	SKYCDBG("%s end : rc = %d\n",__func__, rc);
	
	return rc;
}

static int32_t ce1612_set_led_mode(struct msm_sensor_ctrl_t *s_ctrl, int8_t led_mode)
{
	int rc;
	uint8_t data_buf[4];
	uint8_t read_data =0;

	SKYCDBG("%s: %d E\n",__func__, led_mode);

	if(sensor_mode == 0) // test
		return 0;	 

	if ((led_mode < 0) || (led_mode > 4)) {
		SKYCERR("%s err(-EINVAL)\n", __func__);
		return -EINVAL;
	}

	rc = ce1612_cmd_read(s_ctrl, 0x24, &read_data, 1);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

   // AF-T state check
	rc = ce1612_cmd_read(s_ctrl, 0x2D, data_buf, 2);
	if (rc < 0)
	{
		SKYCERR("%s err(%d)\n", __func__, rc);
		return rc;
	}

	if(data_buf[0] == 0x01)
		read_data = 0;


	if(read_data & 0x01)
	{
		rc = ce1612_lens_stop(s_ctrl);
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}		
	}

	switch(led_mode)
	{
		case 0: //off
			data_buf[0] = 0x00;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(s_ctrl, 0x06, data_buf, 2);

			data_buf[0] = 0x01;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(s_ctrl, 0xB2, data_buf, 2);

			data_buf[0] = 0x03;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(s_ctrl, 0xB2, data_buf, 2);
			break;

		case 2:  
		case 3: 
		case 4:
			data_buf[0] = 0x01;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(s_ctrl, 0xB2, data_buf, 2);

			data_buf[0] = 0x03;
			data_buf[1] = 0x00;
			rc = ce1612_cmd(s_ctrl, 0xB2, data_buf, 2);

			data_buf[0] = 0x00;
			data_buf[1] = 0x00;
			data_buf[2] = 0x00;
			data_buf[3] = 0x00;
			rc = ce1612_cmd(s_ctrl, 0xB3, data_buf, 4);

			data_buf[0] = 0x00;
			data_buf[1] = 0x01;
			rc = ce1612_cmd(s_ctrl, 0x06, data_buf, 2);
			break;

		case 1:
		default:		
			break;			
	}

	if(read_data & 0x01)
	{
		rc = ce1612_cmd(s_ctrl, 0x23, 0, 0);	// AF Start
		if (rc < 0)
		{
			SKYCERR("%s err(%d)\n", __func__, rc);
			return rc;
		}	
	}	
	
	SKYCDBG("%s: X\n",__func__);	
	return rc;
}
#endif


int32_t ce1612_sensor_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int rc = 0;
	struct msm_sensor_ctrl_t *s_ctrl;
	CDBG("%s_i2c_probe called\n", client->name);
	
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		CDBG("i2c_check_functionality failed\n");
		rc = -EFAULT;
		//return rc;
		goto probe_fail;
	}

	s_ctrl = (struct msm_sensor_ctrl_t *)(id->driver_data);
	if (s_ctrl->sensor_i2c_client != NULL) {
		s_ctrl->sensor_i2c_client->client = client;
		if (s_ctrl->sensor_i2c_addr != 0)
			s_ctrl->sensor_i2c_client->client->addr = s_ctrl->sensor_i2c_addr;
	} else {
		rc = -EFAULT;
		//return rc;
		goto probe_fail;
	}

	s_ctrl->sensordata = client->dev.platform_data;

#ifdef F_FW_UPDATE
	// ISP firmware update on probing
	ce1612_update_fw_boot(s_ctrl, s_ctrl->sensordata);
#endif

	snprintf(s_ctrl->sensor_v4l2_subdev.name,
		sizeof(s_ctrl->sensor_v4l2_subdev.name), "%s", id->name);
		v4l2_i2c_subdev_init(&s_ctrl->sensor_v4l2_subdev, client,
		s_ctrl->sensor_v4l2_subdev_ops);

	msm_sensor_register(&s_ctrl->sensor_v4l2_subdev);
	goto i2c_probe_end;
	
probe_fail:
	CDBG("%s_i2c_probe failed\n", client->name);
	
i2c_probe_end:
	return rc;
}


static int __init msm_sensor_init_module(void)
{
	return i2c_add_driver(&ce1612_i2c_driver);
}


static struct v4l2_subdev_core_ops ce1612_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};


static struct v4l2_subdev_core_ops ce1612_subdev_core_ops;
static struct v4l2_subdev_video_ops ce1612_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};


static struct v4l2_subdev_ops ce1612_subdev_ops = {
	.core = &ce1612_subdev_core_ops,
	.video  = &ce1612_subdev_video_ops,
};


static struct msm_sensor_fn_t ce1612_func_tbl = {
#ifdef F_STREAM_ON_OFF		
	.sensor_start_stream = ce1612_sensor_start_stream, //msm_sensor_start_stream,
	.sensor_stop_stream = ce1612_sensor_stop_stream, //msm_sensor_stop_stream,
#endif
#if 0
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_get_prev_lines_pf = msm_sensor_get_prev_lines_pf,
	.sensor_get_prev_pixels_pl = msm_sensor_get_prev_pixels_pl,
	.sensor_get_pict_lines_pf = msm_sensor_get_pict_lines_pf,
	.sensor_get_pict_pixels_pl = msm_sensor_get_pict_pixels_pl,
	.sensor_get_pict_max_exp_lc = msm_sensor_get_pict_max_exp_lc,
	.sensor_get_pict_fps = msm_sensor_get_pict_fps,
	.sensor_set_fps = msm_sensor_set_fps,
	.sensor_write_exp_gain = msm_sensor_write_exp_gain1,
#endif
    .sensor_setting = ce1612_sensor_setting, //msm_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
#ifdef F_CE1612_POWER
	.sensor_power_up = ce1612_sensor_power_up,//msm_sensor_power_up,
	.sensor_power_down = ce1612_sensor_power_down,//msm_sensor_power_down,
#else
    .sensor_power_up = msm_sensor_power_up,
    .sensor_power_down = msm_sensor_power_down,
#endif
	.sensor_get_csi_params = msm_sensor_get_csi_params,
#ifdef CONFIG_PANTECH_CAMERA
    .sensor_set_brightness = ce1612_sensor_set_brightness,
    .sensor_set_effect = ce1612_sensor_set_effect,
    .sensor_set_exposure_mode = ce1612_sensor_set_exposure_mode,
    .sensor_set_wb = ce1612_sensor_set_wb,
    .sensor_set_scene_mode = ce1612_set_scene_mode,
    .sensor_set_preview_fps = ce1612_sensor_set_preview_fps,
    .sensor_set_reflect = ce1612_sensor_set_reflect,    
    .sensor_set_auto_focus = ce1612_sensor_set_auto_focus,
    .sensor_set_antishake = ce1612_set_antishake,
	.sensor_set_antibanding = ce1612_set_antibanding,
    .sensor_check_af = ce1612_sensor_check_af,    
    .sensor_set_coninuous_af = ce1612_set_coninuous_af,
    .sensor_set_focus_rect = ce1612_set_focus_rect,
    .sensor_set_led_mode = ce1612_set_led_mode,
    .sensor_set_wdr = ce1612_set_wdr,
    .sensor_lens_stability = ce1612_lens_stability,
#endif
};

static struct msm_sensor_reg_t ce1612_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
#if 0 
	.start_stream_conf = ce1612_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(ce1612_start_settings),
	.stop_stream_conf = ce1612_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(ce1612_stop_settings),
#endif
#if 0
	.group_hold_on_conf = ce1612_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(ce1612_groupon_settings),
	.group_hold_off_conf = ce1612_groupoff_settings,
	.group_hold_off_conf_size =
		ARRAY_SIZE(ce1612_groupoff_settings),
	.init_settings = &ce1612_init_conf[0],
	.init_size = ARRAY_SIZE(ce1612_init_conf),
	.mode_settings = &ce1612_confs[0],
#endif
	.init_settings = NULL,//&ce1612_init_conf[0],
	.init_size = 0, //ARRAY_SIZE(ce1612_init_conf),
	.mode_settings = NULL, //&ce1612_confs[0],
	.output_settings = &ce1612_dimensions[0],
	.num_conf = ARRAY_SIZE(ce1612_cid_cfg),
};

static struct msm_sensor_ctrl_t ce1612_s_ctrl = {
	.msm_sensor_reg = &ce1612_regs,
	.sensor_i2c_client = &ce1612_sensor_i2c_client,
	.sensor_i2c_addr = 0x7C,
	.sensor_id_info = &ce1612_id_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.csi_params = &ce1612_csi_params_array[0],
	.msm_sensor_mutex = &ce1612_mut,
	.sensor_i2c_driver = &ce1612_i2c_driver,
	.sensor_v4l2_subdev_info = ce1612_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(ce1612_subdev_info),
	.sensor_v4l2_subdev_ops = &ce1612_subdev_ops,
	.func_tbl = &ce1612_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};

late_initcall(msm_sensor_init_module);
MODULE_DESCRIPTION("CE1612 8MP YUV sensor driver");
MODULE_LICENSE("GPL v2");
