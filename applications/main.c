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

rt_tick_t hmc5883l_prev_milli = 0;
rt_tick_t hmc5883l_curr_milli = 0;
const rt_tick_t HMC5883L_DELAY = 1000;

rt_tick_t bme280_prev_milli = 0;
rt_tick_t bme280_curr_milli = 0;
const rt_tick_t BME280_DELAY = 1000;

rt_tick_t ltr390_prev_milli = 0;
rt_tick_t ltr390_curr_milli = 0;
const rt_tick_t LTR390_DELAY = 1000;

// -------- FUNCTION DECLARATION --------

// -------- HAL VARIABLES --------

// -------- HAL FUNCTION DECLARATION --------

// -------- TMP --------

int main(void)
{
	int count = 1;

	// led_embedded_init();
	// key_embedded_init();

	// struct hmc5883l_device_struct *hmc5883l = RT_NULL;
	// struct hmc5883l_data_struct hmc5883l_rawData;
	// hmc5883l = my_hmc5883l_init(HMC5883L_I2C_BUS, HMC5883L_ADDR);

	// struct bme280_device_struct *bme280 = RT_NULL;
	// struct bme280_data bme280_rawData;
	// bme280 = my_bme280_init(BME280_I2C_BUS, BME280_ADDR);

	// struct ltr390_device_struct *ltr390 = RT_NULL;
	// struct ltr390_data_struct ltr390_rawData;
	// ltr390 = my_ltr390_init(LTR390_I2C_BUS, LTR390_ADDR);

	ILI9341_Init(); //LCD 初始化
	//其中0、3、5、6 模式适合从左至右显示文字，
	//不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果
	//其中 6 模式为大部分液晶例程的默认显示方向
	// ILI9341_GramScan(6);

	while (count++) {
		// if (key1_isPressed()) {
		// 	led_isOn = RT_TRUE;
		// } else if (key2_isPressed()) {
		// 	led_isOn = RT_FALSE;
		// }
		// if (led_isOn) {
		// 	led_embedded_blink(50);
		// } else {
		// 	led_embedded_off();
		// }

		// hmc5883l_curr_milli = rt_tick_get_millisecond();
		// if ((hmc5883l_curr_milli - hmc5883l_prev_milli >= HMC5883L_DELAY) && (hmc5883l != RT_NULL)) {
		// 	hmc5883l_prev_milli = hmc5883l_curr_milli;
		// 	if (my_hmc5883l_get_data(hmc5883l, &hmc5883l_rawData) == RT_EOK) {
		// 		LOG_D("x: %d", hmc5883l_rawData.x);
		// 		LOG_D("y: %d", hmc5883l_rawData.y);
		// 		LOG_D("z: %d", hmc5883l_rawData.z);
		// 	} else {
		// 		LOG_E("HMC5883L get data failed!");
		// 		my_hmc5883l_destroy(hmc5883l);
		// 	}
		// }

		// bme280_curr_milli = rt_tick_get_millisecond();
		// if ((bme280_curr_milli - bme280_prev_milli >= BME280_DELAY) && (bme280 != RT_NULL)) {
		// 	bme280_prev_milli = bme280_curr_milli;
		// 	if (my_bme280_get_data(bme280, &bme280_rawData) == RT_EOK) {
		// 		LOG_D("temperature: %0.2f", bme280_rawData.temperature);
		// 		LOG_D("humidity: %0.2f", bme280_rawData.humidity);
		// 		LOG_D("baro pressure: %0.2f", bme280_rawData.pressure / 100);
		// 	} else {
		// 		LOG_E("BME280 get data failed!");
		// 		my_bme280_destroy(bme280);
		// 	}
		// }

		// ltr390_curr_milli = rt_tick_get_millisecond();
		// if ((ltr390_curr_milli - ltr390_prev_milli >= LTR390_DELAY) && (ltr390 != RT_NULL)) {
		// 	ltr390_prev_milli = ltr390_curr_milli;
		// 	if (my_ltr390_get_data(ltr390, &ltr390_rawData) == RT_EOK) {
		// 		LOG_D("lux: %0.2f", ltr390_rawData.lux);
		// 		LOG_D("uvi: %d", ltr390_rawData.uvi);
		// 	} else {
		// 		LOG_E("LTR390 get data failed!");
		// 		my_ltr390_destroy(ltr390);
		// 	}
		// }

		// LCD_Test();
		LCD_Direction_Show();
	}

	return RT_EOK;
}
