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

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_renesas_hd.h"

#define MIPI_CLOCK_410BPS

static struct msm_panel_info pinfo;

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
/* regulator */
{0x03, 0x0a, 0x04, 0x00, 0x20},
/* timing */
{0xab, 0x8a, 0x18, 0x00, 0x92, 0x97, 0x1b, 0x8c,
 0x0c, 0x03, 0x04, 0xa0},
/* phy ctrl */
{0x5f, 0x00, 0x00, 0x10},
/* strength */
{0xff, 0x00, 0x06, 0x00},
/* pll control */
#ifdef MIPI_CLOCK_410BPS
{0x0, 0x98, 0x31, 0xda, 0x00, 0x50, 0x48, 0x63,
#else
{0x0, 0x74, 0x31, 0xda, 0x00, 0x50, 0x48, 0x63,
#endif
 0x31, 0x0f, 0x03,
 0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};

static int __init mipi_video_renesas_hd_init(void)
{
	int ret;

#ifdef CONFIG_FB_MSM_MIPI_PANEL_DETECT
	if (msm_fb_detect_client("mipi_video_renesas_hd"))
		return 0;
#endif

    pinfo.xres = 720;
    pinfo.yres = 1280;

	/*
	 *
	 * Panel's Horizontal input timing requirement is to
	 * include dummy(pad) data of 200 clk in addition to
	 * width and porch/sync width values
	 */
	//pinfo.mipi.xres_pad = 200;
	//pinfo.mipi.yres_pad = 0;

//	pinfo.mipi.xres_pad = 0;
//	pinfo.mipi.yres_pad = 0;

	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;

#ifdef MIPI_CLOCK_410BPS
	pinfo.lcdc.h_back_porch = 18;
	pinfo.lcdc.h_front_porch = 156;
    	pinfo.lcdc.h_pulse_width = 4;
#else
	pinfo.lcdc.h_back_porch = 20;
    	pinfo.lcdc.h_front_porch = 60;
    	pinfo.lcdc.h_pulse_width = 2;

#endif
    pinfo.lcdc.v_back_porch = 3;
    pinfo.lcdc.v_front_porch = 5;
    pinfo.lcdc.v_pulse_width = 2;

	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 10;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	
	pinfo.mipi.mode = DSI_VIDEO_MODE;    
	
    pinfo.mipi.hfp_power_stop = FALSE;
    pinfo.mipi.hbp_power_stop = FALSE;

    pinfo.mipi.hsa_power_stop = TRUE;
	pinfo.mipi.pulse_mode_hsa_he = TRUE;

    pinfo.mipi.eof_bllp_power_stop = FALSE;
    pinfo.mipi.bllp_power_stop = FALSE;

	pinfo.mipi.traffic_mode = DSI_NON_BURST_SYNCH_EVENT;

	pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
    pinfo.mipi.data_lane0 = TRUE;
    pinfo.mipi.data_lane1 = TRUE;
    pinfo.mipi.data_lane2 = TRUE;
    pinfo.mipi.data_lane3 = TRUE;

	pinfo.mipi.t_clk_post = 0x22;
#ifdef MIPI_CLOCK_410BPS
 	pinfo.mipi.t_clk_pre = 0x2F;
        pinfo.clk_rate = 410000000;
        pinfo.mipi.frame_rate = 59;
#else
 	pinfo.mipi.t_clk_pre = 0x2E;
        pinfo.clk_rate = 374000000;
        pinfo.mipi.frame_rate = 60;
#endif
	pinfo.mipi.stream = 0; /* dma_p */
	pinfo.mipi.mdp_trigger = 0;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo.mipi.tx_eot_append = TRUE;

	ret = mipi_renesas_hd_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_WVGA_PT);
	if (ret)
		printk(KERN_ERR "%s: failed to register device!\n", __func__);

	return ret;
}
module_init(mipi_video_renesas_hd_init);

