/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-04-22     AZ           INIT
 */

#include "main.h"

// -------- VARIABLES --------
rt_bool_t led_isOn = RT_FALSE;

struct hmc5883l_device_struct *hmc5883l = RT_NULL;
struct hmc5883l_data_struct hmc5883l_data;
rt_tick_t hmc5883l_prev_milli = 0;
rt_tick_t hmc5883l_curr_milli = 0;
const rt_tick_t HMC5883L_DELAY = 1000;

// -------- HAL VARIABLES --------

// -------- FUNCTION DECLARATION --------

// -------- HAL FUNCTION DECLARATION --------

int main(void)
{
	int count = 1;

	led_embedded_init();
	key_embedded_init();
	hmc5883l = hmc5883l_init(HMC5883L_I2C_BUS, HMC5883L_ADDR);

	while (count++) {
		if (key1_isPressed()) {
			led_isOn = RT_TRUE;
		} else if (key2_isPressed()) {
			led_isOn = RT_FALSE;
		}
		if (led_isOn) {
			led_embedded_blink(50);
		} else {
			led_embedded_off();
		}

		hmc5883l_curr_milli = rt_tick_get_millisecond();
		if (hmc5883l_curr_milli - hmc5883l_prev_milli >= HMC5883L_DELAY) {
			hmc5883l_prev_milli = hmc5883l_curr_milli;
			hmc5883l_get_data(hmc5883l, &hmc5883l_data);
			LOG_D("x: %d", hmc5883l_data.x);
			LOG_D("y: %d", hmc5883l_data.y);
			LOG_D("z: %d", hmc5883l_data.z);
		}
	}

	return RT_EOK;
}
