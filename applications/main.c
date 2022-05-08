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
// for outputing debugging info
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

// -------- VARIABLES --------
rt_bool_t led_isOn = RT_FALSE;

rt_tick_t hmc5883l_prev_milli = 0;
rt_tick_t hmc5883l_curr_milli = 0;
const rt_tick_t HMC5883L_DELAY = 1000;
struct hmc5883l_device_struct *hmc5883l = RT_NULL;
struct hmc5883l_data_struct hmc5883l_rawData;

rt_tick_t bme280_prev_milli = 0;
rt_tick_t bme280_curr_milli = 0;
const rt_tick_t BME280_DELAY = 1000;
struct bme280_device_struct *bme280 = RT_NULL;
struct bme280_data bme280_rawData;

rt_tick_t ltr390_prev_milli = 0;
rt_tick_t ltr390_curr_milli = 0;
const rt_tick_t LTR390_DELAY = 1000;
struct ltr390_device_struct *ltr390 = RT_NULL;
struct ltr390_data_struct ltr390_rawData;

rt_tick_t gp2y1014au0f_prev_milli = 0;
rt_tick_t gp2y1014au0f_curr_milli = 0;
const rt_tick_t GP2Y1014AU0F_DELAY = 1000;
// struct gp2y1014au0f_device_struct *gp2y1014au0f = RT_NULL; // !!! don't use
double gp2y1014au0f_rawData;

rt_tick_t lm386_prev_milli = 0;
rt_tick_t lm386_curr_milli = 0;
const rt_tick_t LM386_DELAY = 1000;
// struct lm386_device_struct *lm386 = RT_NULL; // !!! don't use
double lm386_rawData;

// extern uint8_t Ov7725_vsync;
// extern OV7725_MODE_PARAM cam_mode;

// -------- FUNCTION DECLARATION --------
void my_key_and_led_test(void);
void my_hmc5883l_test(void);
void my_bme280_test(void);
void my_ltr390_test(void);
void my_gp2y1014au0f_test(void);
void my_lm386_test(void);

// -------- HAL VARIABLES --------

// -------- HAL FUNCTION DECLARATION --------

// -------- TMP --------

int main(void)
{
	int count = 1;

	led_embedded_init();
	key_embedded_init();
	hmc5883l = my_hmc5883l_init(HMC5883L_I2C_BUS, HMC5883L_ADDR);
	bme280 = my_bme280_init(BME280_I2C_BUS, BME280_ADDR);
	ltr390 = my_ltr390_init(LTR390_I2C_BUS, LTR390_ADDR);
	my_gp2y1014au0f_init();
	// lm386 = my_lm386_init(LM386_ADC_DEV_NAME, LM386_ADC_DEV_CHANNEL); // !!! don't use
	my_lm386_init();
	ILI9341_Init();
	XPT2046_Init();
	ILI9341_GramScan(3);
	// Palette_Init(LCD_SCAN_MODE); // 绘制触摸画板界面
	my_ov7725_test_init();

	while (count++) {
		// my_key_and_led_test();
		my_hmc5883l_test();
		my_bme280_test();
		my_ltr390_test();
		my_gp2y1014au0f_test();
		my_lm386_test();
		// LCD_Test();
		// LCD_Direction_Show();
		// XPT2046_TouchEvenHandler();
		my_ov7725_test();
	}

	return RT_EOK;
}

void my_key_and_led_test(void)
{
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
};

void my_hmc5883l_test(void)
{
	hmc5883l_curr_milli = rt_tick_get_millisecond();
	if ((hmc5883l_curr_milli - hmc5883l_prev_milli >= HMC5883L_DELAY) && (hmc5883l != RT_NULL)) {
		hmc5883l_prev_milli = hmc5883l_curr_milli;
		if (my_hmc5883l_get_data(hmc5883l, &hmc5883l_rawData) == RT_EOK) {
			LOG_D("x: %d", hmc5883l_rawData.x);
			LOG_D("y: %d", hmc5883l_rawData.y);
			LOG_D("z: %d", hmc5883l_rawData.z);
		} else {
			LOG_E("HMC5883L get data failed!");
			my_hmc5883l_destroy(hmc5883l);
		}
	}
};

void my_bme280_test(void)
{
	bme280_curr_milli = rt_tick_get_millisecond();
	if ((bme280_curr_milli - bme280_prev_milli >= BME280_DELAY) && (bme280 != RT_NULL)) {
		bme280_prev_milli = bme280_curr_milli;
		if (my_bme280_get_data(bme280, &bme280_rawData) == RT_EOK) {
			LOG_D("temperature: %0.2f", bme280_rawData.temperature);
			LOG_D("humidity: %0.2f", bme280_rawData.humidity);
			LOG_D("baro pressure: %0.2f", bme280_rawData.pressure / 100);
		} else {
			LOG_E("BME280 get data failed!");
			my_bme280_destroy(bme280);
		}
	}
};

void my_ltr390_test(void)
{
	ltr390_curr_milli = rt_tick_get_millisecond();
	if ((ltr390_curr_milli - ltr390_prev_milli >= LTR390_DELAY) && (ltr390 != RT_NULL)) {
		ltr390_prev_milli = ltr390_curr_milli;
		if (my_ltr390_get_data(ltr390, &ltr390_rawData) == RT_EOK) {
			LOG_D("lux: %0.2f", ltr390_rawData.lux);
			LOG_D("uvi: %d", ltr390_rawData.uvi);
		} else {
			LOG_E("LTR390 get data failed!");
			my_ltr390_destroy(ltr390);
		}
	}
};

void my_gp2y1014au0f_test(void)
{
	gp2y1014au0f_curr_milli = rt_tick_get_millisecond();
	if ((gp2y1014au0f_curr_milli - gp2y1014au0f_prev_milli >= GP2Y1014AU0F_DELAY)) {
		gp2y1014au0f_prev_milli = gp2y1014au0f_curr_milli;
		// if (my_gp2y1014au0f_get_data(gp2y1014au0f, &gp2y1014au0f_rawData) == RT_EOK) {
		// 	LOG_D("lux: %d", gp2y1014au0f_rawData);
		// } else {
		// 	LOG_E("GP2Y1014AU0F get data failed!");
		// 	my_gp2y1014au0f_destroy(gp2y1014au0f);
		// }
		gp2y1014au0f_rawData = my_gp2y1014au0f_get_data();
	}
};

void my_lm386_test(void)
{
	lm386_curr_milli = rt_tick_get_millisecond();
	if ((lm386_curr_milli - lm386_prev_milli >= LM386_DELAY)) {
		lm386_prev_milli = lm386_curr_milli;
		// if (my_lm386_get_data(lm386, &lm386_rawData) == RT_EOK) {
		// 	LOG_D("lux: %d", lm386_rawData);
		// } else {
		// 	LOG_E("LM386 get data failed!");
		// 	my_lm386_destroy(lm386);
		// }
		lm386_rawData = my_lm386_get_data();
	}
};
