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

//++ p11309
#if defined(CONFIG_MACH_MSM8960_SIRIUSLTE) 
	#include "siriuslte/cyttsp4_pm8921.h"
#else
	#include "ef48/cyttsp4_pm8921.h"
#endif
//-- p11309

int cyttsp4_init_hw_setting(struct device *dev)
{	
	int rc =0;

#if CY_USE_LDO_POWER
#if CY_USE_LDO_POWER_PMIC_GPIO
	
	printk("%s: Use LDO Power...0...with PMIC GPIO\n", __func__);

	rc = gpio_request(PM8921_GPIO_PM_TO_SYS(GPIO_TOUCH_LDO1), "touch_ldo1");
	if (rc) {
		printk("gpio_request GPIO_TOUCH_LDO1 failed, rc=%d\n",rc);
		return -EINVAL;
	}
	rc = gpio_direction_output(PM8921_GPIO_PM_TO_SYS(GPIO_TOUCH_LDO1), 1);
	if (rc) {
		printk("gpio_direction_output GPIO_TOUCH_LDO1 failed, rc=%d\n", rc);
		return -EINVAL;
	}
#else

	printk("%s: Use LDO Power...0...with MSM GPIO\n", __func__);
	
	gpio_request(GPIO_TOUCH_LDO1, "touch_power_n");	
	rc = gpio_tlmm_config(
		GPIO_CFG(GPIO_TOUCH_LDO1, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), 
		GPIO_CFG_ENABLE
	);

	if (rc) {
		printk(KERN_ERR "%s: GPIO_TOUCH_LDO1 failed (%d)\n",__func__, rc);
		return -EINVAL;
	}
	gpio_set_value(GPIO_TOUCH_LDO1, 1);

#endif

#if CY_USE_LDO_PIN_COUNT
#if CY_USE_LDO_POWER_PMIC_GPIO

	printk("%s: Use LDO Power...1...with PMIC GPIO\n", __func__);

	rc = gpio_request(PM8921_GPIO_PM_TO_SYS(GPIO_TOUCH_LDO2), "touch_ldo2");
	if (rc) {
		printk("gpio_request GPIO_TOUCH_LDO2 failed, rc=%d\n", rc);
		return -EINVAL;
	}
	rc = gpio_direction_output(PM8921_GPIO_PM_TO_SYS(GPIO_TOUCH_LDO2), 1);
	if (rc) {
		printk("gpio_direction_output GPIO_TOUCH_LDO2 failed, rc=%d\n",rc);
		return -EINVAL;
	}
#else
#endif

	printk("%s: Use LDO Power...1...with MSM GPIO\n", __func__);

	gpio_request(GPIO_TOUCH_LDO2, "touch_power_n");	
	rc = gpio_tlmm_config(
		GPIO_CFG(GPIO_TOUCH_LDO2, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), 
		GPIO_CFG_ENABLE
	);

	if (rc) {
		printk(KERN_ERR "%s: GPIO_TOUCH_LDO2 failed (%d)\n",__func__, rc);
		return -EINVAL;
	}
	gpio_set_value(GPIO_TOUCH_LDO2, 1);

#endif

#else
	
	struct regulator *vreg_touch_3_3, *vreg_touch_1_8;

#if defined(CONFIG_MACH_APQ8064_EF50L)
#if CONFIG_BOARD_VER < CONFIG_WS20
	struct regulator *vreg_touch_1_8_50L;
	vreg_touch_1_8_50L = regulator_get(NULL, "8921_lvs5");
	rc = regulator_enable(vreg_touch_1_8_50L);
#endif /* --CONFIG_BOARD_VER < CONFIG_WS20 */
#endif /* --CONFIG_MACH_APQ8064_EF50L */

	printk("%s: Use PMIC Power...1\n", __func__);

	vreg_touch_1_8 = regulator_get(NULL, TOUCH_POWER_VCC);
	rc = regulator_set_voltage(vreg_touch_1_8, 1800000, 1800000);
	if (rc) { 
		printk("set_voltage 8921_l21 failed, rc=%d\n", rc);
		return -EINVAL;
	}

	vreg_touch_3_3 = regulator_get(NULL, TOUCH_POWER_VDD);
	rc = regulator_set_voltage(vreg_touch_3_3, 2900000, 2900000);
	if (rc) { 
		printk("set_voltage 8921_l17 failed, rc=%d\n", rc);
		return -EINVAL;
	}

	rc = regulator_enable(vreg_touch_1_8);
	if (rc) { 
		printk("regulator_enable vreg_touch_1_8 failed, rc=%d\n", rc);
		return -EINVAL;
	}

	rc = regulator_enable(vreg_touch_3_3);
	if (rc) { 
		printk("regulator_enable vreg_touch_3_3 failed, rc=%d\n", rc);
		return -EINVAL;
	}

#endif

	rc = gpio_request(GPIO_TOUCH_RST, "touch_rst");
	if (rc) {
		printk("gpio_request GPIO_TOUCH_RST : %d failed, rc=%d\n",GPIO_TOUCH_RST, rc);
		return -EINVAL;
	}

	rc = gpio_direction_output(GPIO_TOUCH_RST, 1);
	if (rc) {
		printk("gpio_direction_output GPIO_TOUCH_RST : %d failed, rc=%d\n",GPIO_TOUCH_RST, rc);
		return -EINVAL;
	}

	rc = gpio_request(GPIO_TOUCH_CHG, "touch_chg");
	if (rc) {
		printk("gpio_request GPIO_TOUCH_CHG : %d failed, rc=%d\n",GPIO_TOUCH_CHG, rc);
		return -EINVAL;
	}  

	rc = gpio_direction_input(GPIO_TOUCH_CHG);
	if (rc) {
		printk("gpio_direction_input gpio_chg : %d failed, rc=%d\n",GPIO_TOUCH_CHG, rc);
		return -EINVAL;
	}

	msleep(5);
	return 0;
}
EXPORT_SYMBOL_GPL(cyttsp4_init_hw_setting);