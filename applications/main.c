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

// -------- FUNCTION DECLARATION --------

int main(void)
{
	int count = 1;
	led_embedded_init();
	key_embedded_init();

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

		rt_thread_mdelay(1000);
	}

	return RT_EOK;
}
