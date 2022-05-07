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

rt_tick_t bme280_prev_milli = 0;
rt_tick_t bme280_curr_milli = 0;
const rt_tick_t BME280_DELAY = 1000;

rt_tick_t ltr390_prev_milli = 0;
rt_tick_t ltr390_curr_milli = 0;
const rt_tick_t LTR390_DELAY = 1000;

extern uint8_t Ov7725_vsync;
extern OV7725_MODE_PARAM cam_mode;

// -------- FUNCTION DECLARATION --------

// -------- HAL VARIABLES --------

// -------- HAL FUNCTION DECLARATION --------

// -------- TMP --------

int main(void)
{
	int count = 1;

	led_embedded_init();
	key_embedded_init();

	struct hmc5883l_device_struct *hmc5883l = RT_NULL;
	struct hmc5883l_data_struct hmc5883l_rawData;
	hmc5883l = my_hmc5883l_init(HMC5883L_I2C_BUS, HMC5883L_ADDR);

	struct bme280_device_struct *bme280 = RT_NULL;
	struct bme280_data bme280_rawData;
	bme280 = my_bme280_init(BME280_I2C_BUS, BME280_ADDR);

	struct ltr390_device_struct *ltr390 = RT_NULL;
	struct ltr390_data_struct ltr390_rawData;
	ltr390 = my_ltr390_init(LTR390_I2C_BUS, LTR390_ADDR);

	ILI9341_Init();
	// XPT2046_Init();
	// //其中0、3、5、6 模式适合从左至右显示文字，
	// //不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果
	// //其中 6 模式为大部分液晶例程的默认显示方向
	ILI9341_GramScan(3);
	// //绘制触摸画板界面
	// Palette_Init(LCD_SCAN_MODE);

	float frame_count = 0;
	uint8_t retry = 0;
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED, BLACK);
	ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH); /* 清屏，显示全黑 */
	/********显示字符串示例*******/
	ILI9341_DispStringLine_EN(LINE(0), "BH OV7725 Test Demo");
	/* ov7725 gpio 初始化 */
	OV7725_GPIO_Config();
	led_embedded_color(LED_BLUE);
	/* ov7725 寄存器默认配置初始化 */
	while (OV7725_Init() != SUCCESS) {
		retry++;
		if (retry > 5) {
			ILI9341_DispStringLine_EN(LINE(2), "No OV7725 module detected!");
			while (1)
				;
		}
	}
	/*根据摄像头参数组配置模式*/
	OV7725_Special_Effect(cam_mode.effect);
	/*光照模式*/
	OV7725_Light_Mode(cam_mode.light_mode);
	/*饱和度*/
	OV7725_Color_Saturation(cam_mode.saturation);
	/*光照度*/
	OV7725_Brightness(cam_mode.brightness);
	/*对比度*/
	OV7725_Contrast(cam_mode.contrast);
	/*特殊效果*/
	OV7725_Special_Effect(cam_mode.effect);
	/*设置图像采样及模式大小*/
	OV7725_Window_Set(cam_mode.cam_sx, cam_mode.cam_sy, cam_mode.cam_width, cam_mode.cam_height, cam_mode.QVGA_VGA);
	/* 设置液晶扫描模式 */
	ILI9341_GramScan(cam_mode.lcd_scan);
	ILI9341_DispStringLine_EN(LINE(2), "OV7725 initialize success!");

	Ov7725_vsync = 0;

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
		// LCD_Direction_Show();
		// XPT2046_TouchEvenHandler();

		/*接收到新图像进行显示*/
		if (Ov7725_vsync == 2) {
			frame_count++;
			FIFO_PREPARE; /*FIFO准备*/
			ImagDisp(cam_mode.lcd_sx, cam_mode.lcd_sy, cam_mode.cam_width, cam_mode.cam_height); /*采集并显示*/

			Ov7725_vsync = 0;
			led_embedded_color(LED_RED);
		}

		/*检测按键*/
		if (key1_isPressed()) {
			/*LED反转*/
			led_embedded_color(LED_GREEN);
		}
		/*检测按键*/
		if (key2_isPressed()) {
			/*LED反转*/
			led_embedded_color(LED_BLUE);

			/*动态配置摄像头的模式，
			有需要可以添加使用串口、用户界面下拉选择框等方式修改这些变量，
			达到程序运行时更改摄像头模式的目的*/
			cam_mode.QVGA_VGA = 0, //QVGA模式
				cam_mode.cam_sx = 0, cam_mode.cam_sy = 0,

			cam_mode.cam_width = 320, cam_mode.cam_height = 240,

			cam_mode.lcd_sx = 0, cam_mode.lcd_sy = 0, cam_mode.lcd_scan = 3,
			//LCD扫描模式，本横屏配置可用1、3、5、7模式

				//以下可根据自己的需要调整，参数范围见结构体类型定义
				cam_mode.light_mode = 0, //自动光照模式
				cam_mode.saturation = 0, cam_mode.brightness = 0, cam_mode.contrast = 0,
			cam_mode.effect = 1, //黑白模式

				/*根据摄像头参数写入配置*/
				OV7725_Special_Effect(cam_mode.effect);
			/*光照模式*/
			OV7725_Light_Mode(cam_mode.light_mode);
			/*饱和度*/
			OV7725_Color_Saturation(cam_mode.saturation);
			/*光照度*/
			OV7725_Brightness(cam_mode.brightness);
			/*对比度*/
			OV7725_Contrast(cam_mode.contrast);
			/*特殊效果*/
			OV7725_Special_Effect(cam_mode.effect);

			/*设置图像采样及模式大小*/
			OV7725_Window_Set(cam_mode.cam_sx, cam_mode.cam_sy, cam_mode.cam_width, cam_mode.cam_height,
					  cam_mode.QVGA_VGA);

			/* 设置液晶扫描模式 */
			ILI9341_GramScan(cam_mode.lcd_scan);
		}
	}

	return RT_EOK;
}
