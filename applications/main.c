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
// init
rt_uint32_t initCount = 0;
// led
rt_bool_t led_isOn = RT_FALSE;
// key
extern rt_bool_t key1_isChanged;
extern rt_bool_t key2_isChanged;
// hmc5883l
rt_tick_t hmc5883l_prev_milli = 0;
rt_tick_t hmc5883l_curr_milli = 0;
const rt_tick_t HMC5883L_DELAY = 1000;
struct hmc5883l_device_struct *hmc5883l = RT_NULL;
struct hmc5883l_data_struct hmc5883l_rawData;
double hmc5883l_heading;
// bme280
rt_tick_t bme280_prev_milli = 0;
rt_tick_t bme280_curr_milli = 0;
const rt_tick_t BME280_DELAY = 1000;
struct bme280_device_struct *bme280 = RT_NULL;
struct bme280_data bme280_rawData;
// ltr390
rt_tick_t ltr390_prev_milli = 0;
rt_tick_t ltr390_curr_milli = 0;
const rt_tick_t LTR390_DELAY = 1000;
struct ltr390_device_struct *ltr390 = RT_NULL;
struct ltr390_data_struct ltr390_rawData;
// gp2y1014au0f
rt_tick_t gp2y1014au0f_prev_milli = 0;
rt_tick_t gp2y1014au0f_curr_milli = 0;
const rt_tick_t GP2Y1014AU0F_DELAY = 1000;
// struct gp2y1014au0f_device_struct *gp2y1014au0f = RT_NULL; // !!! don't use
double gp2y1014au0f_rawData;
// lm386
rt_tick_t lm386_prev_milli = 0;
rt_tick_t lm386_curr_milli = 0;
const rt_tick_t LM386_DELAY = 10;
// struct lm386_device_struct *lm386 = RT_NULL; // !!! don't use
double lm386_rawData;
// ov7725
extern uint8_t Ov7725_vsync;
extern OV7725_MODE_PARAM cam_mode;
// sd card
rt_bool_t isSavingData = RT_FALSE;
// scene
rt_bool_t scene_isChanged = RT_TRUE;
// menu
typedef enum {
	SCENE_MENU = 0,
	SCENE_DISP_DATA = 1,
	SCENE_TAKE_PHOTO = 2,
	SCENE_SD_CARD = 3,
	SCENE_SETTINGS = 4,
} SCENE;
SCENE currScene = SCENE_MENU;
SCENE prevScene = SCENE_MENU;
SCENE selectScene = SCENE_DISP_DATA;
rt_bool_t select_isChanged = RT_FALSE;
// display data
rt_tick_t DISP_DATA_prev_milli = 0;
rt_tick_t DISP_DATA_curr_milli = 0;
const rt_tick_t DISP_DATA_DELAY = 1000;
// take photo
rt_bool_t takePhoto_isSuccess = RT_TRUE;
// sd card
rt_bool_t isEjecting = RT_FALSE;
rt_bool_t isEjectSuccess = RT_TRUE;

// -------- FUNCTION DECLARATION --------
rt_err_t mainFuncInit(void);
rt_err_t mainFunc(void);
// store data to sd card
void sdcard_saveCurrData(void);
// scene tasks
void MENU_task(void);
void MENU_updateSelect(void);
void DISP_DATA_task(void);
void DISP_DATA_updateData(void);
void TAKE_PHOTO_task(void);
void SD_CARD_task(void);
void SETTINGS_task(void);
// key interrupts for different scenes
void key1_irq_MENU(void);
void key2_irq_MENU(void);
void key1_irq_DISP_DATA(void);
void key2_irq_DISP_DATA(void);
void key1_irq_TAKE_PHOTO(void);
void key2_irq_TAKE_PHOTO(void);
void key1_irq_SD_CARD(void);
void key2_irq_SD_CARD(void);
void key1_irq_SETTING(void);
void key2_irq_SETTING(void);
// my tests
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
	// init LCD
	ILI9341_Init();
	XPT2046_Init();
	ILI9341_GramScan(3);
	// Palette_Init(LCD_SCAN_MODE); // 绘制触摸画板界面

	while (mainFuncInit() != RT_EOK) {
		rt_thread_mdelay(1000);
	}

	// init scene
	currScene = SCENE_MENU;
	prevScene = SCENE_MENU;
	selectScene = SCENE_DISP_DATA;
	scene_isChanged = RT_TRUE;

	int count = 1;
	while (count++) {
		// my_key_and_led_test();
		my_hmc5883l_test();
		my_bme280_test();
		my_ltr390_test();
		my_gp2y1014au0f_test();
		my_lm386_test();
		// LCD_Direction_Show();
		// XPT2046_TouchEvenHandler();
		// my_ov7725_test();

		// LOG_D("This is main");
		// rt_thread_mdelay(5000);

		if (mainFunc() != RT_EOK) {
			// TODO
		}
	}

	return RT_EOK;
}

rt_err_t mainFuncInit(void)
{
	initCount++;

	rt_err_t initStatus = RT_EOK;
	LCD_SetBackColor(CL_BLACK);
	ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);

	// init error message
	LCD_SetFont(&Font16x24);
	LCD_SetTextColor(CL_CYAN);
	ILI9341_DispStringLine_EN(LINE(0), "Initializing...");

	// init led, key, sensor
	LCD_SetFont(&Font8x16);
	LCD_SetTextColor(CL_GREEN);

	led_embedded_init();
	led_embedded_color(LED_GREEN);
	ILI9341_DispStringLine_EN(LINE(2), "(1/9) LED init succeeded!");

	led_embedded_color(LED_BLUE);
	key_embedded_init();
	led_embedded_color(LED_GREEN);
	ILI9341_DispStringLine_EN(LINE(3), "(2/9) KEY init succeeded!");

	led_embedded_color(LED_BLUE);
	hmc5883l = my_hmc5883l_init(HMC5883L_I2C_BUS, HMC5883L_ADDR);
	if (hmc5883l == RT_NULL) {
		initStatus = -RT_ERROR;
		LCD_SetTextColor(CL_RED);
		led_embedded_color(LED_RED);
		ILI9341_DispStringLine_EN(LINE(4), "(3/9) HMC5883L init failed!");
		LCD_SetTextColor(CL_GREEN);
	} else {
		led_embedded_color(LED_GREEN);
		ILI9341_DispStringLine_EN(LINE(4), "(3/9) HMC5883L init succeeded!");
	}

	led_embedded_color(LED_BLUE);
	bme280 = my_bme280_init(BME280_I2C_BUS, BME280_ADDR);
	if (bme280 == RT_NULL) {
		initStatus = -RT_ERROR;
		LCD_SetTextColor(CL_RED);
		led_embedded_color(LED_RED);
		ILI9341_DispStringLine_EN(LINE(5), "(4/9) BME280 init failed!");
		LCD_SetTextColor(CL_GREEN);
	} else {
		led_embedded_color(LED_GREEN);
		ILI9341_DispStringLine_EN(LINE(5), "(4/9) BME280 init succeeded!");
	}

	led_embedded_color(LED_BLUE);
	ltr390 = my_ltr390_init(LTR390_I2C_BUS, LTR390_ADDR);
	if (ltr390 == RT_NULL) {
		initStatus = -RT_ERROR;
		LCD_SetTextColor(CL_RED);
		led_embedded_color(LED_RED);
		ILI9341_DispStringLine_EN(LINE(6), "(5/9) LTR390 init failed!");
		LCD_SetTextColor(CL_GREEN);
	} else {
		led_embedded_color(LED_GREEN);
		ILI9341_DispStringLine_EN(LINE(6), "(5/9) LTR390 init succeeded!");
	}

	led_embedded_color(LED_BLUE);
	my_gp2y1014au0f_init();
	led_embedded_color(LED_GREEN);
	ILI9341_DispStringLine_EN(LINE(7), "(6/9) GP2Y1014AU0F init succeeded!");

	led_embedded_color(LED_BLUE);
	// lm386 = my_lm386_init(LM386_ADC_DEV_NAME, LM386_ADC_DEV_CHANNEL); // !!! don't use
	my_lm386_init();
	led_embedded_color(LED_GREEN);
	ILI9341_DispStringLine_EN(LINE(8), "(7/9) LM386 init succeeded!");

	// init camera
	led_embedded_color(LED_BLUE);
	if (my_ov7725_test_init() != RT_EOK) {
		initStatus = -RT_ERROR;
		LCD_SetTextColor(CL_RED);
		led_embedded_color(LED_RED);
		ILI9341_DispStringLine_EN(LINE(9), "(8/9) OV7725 init failed!");
		LCD_SetTextColor(CL_GREEN);
	} else {
		led_embedded_color(LED_GREEN);
		ILI9341_DispStringLine_EN(LINE(9), "(8/9) OV7725 init succeeded!");
	}

	if (initStatus == RT_EOK) {
		// init SD card
		led_embedded_color(LED_BLUE);
		if (sdcard_init() != RT_EOK) {
			initStatus = -RT_ERROR;
			LCD_SetTextColor(CL_RED);
			led_embedded_color(LED_RED);
			ILI9341_DispStringLine_EN(LINE(10), "(9/9) SD card init failed!");
			LCD_SetTextColor(CL_GREEN);
		} else {
			led_embedded_color(LED_GREEN);
			ILI9341_DispStringLine_EN(LINE(10), "(9/9) SD card init succeeded!");
		}
	} else {
		initStatus = -RT_ERROR;
		LCD_SetTextColor(CL_YELLOW);
		ILI9341_DispStringLine_EN(LINE(10), "(9/9) Skip SD card.");
		LCD_SetTextColor(CL_GREEN);
	}

	return initStatus;
}

rt_err_t mainFunc(void)
{
	switch (currScene) {
	case SCENE_MENU:
		if (scene_isChanged) {
			scene_isChanged = RT_FALSE;

			// rebind key interrupt
			rt_pin_detach_irq(KEY1_EMBEDDED);
			rt_pin_attach_irq(KEY1_EMBEDDED, PIN_IRQ_MODE_RISING, key1_irq_MENU, RT_NULL);
			rt_pin_irq_enable(KEY1_EMBEDDED, PIN_IRQ_ENABLE);
			rt_pin_detach_irq(KEY2_EMBEDDED);
			rt_pin_attach_irq(KEY2_EMBEDDED, PIN_IRQ_MODE_RISING, key2_irq_MENU, RT_NULL);
			rt_pin_irq_enable(KEY2_EMBEDDED, PIN_IRQ_ENABLE);

			MENU_task();
			return RT_EOK;

		} else if (select_isChanged) {
			select_isChanged = RT_FALSE;

			MENU_updateSelect();
			return RT_EOK;

		} else {
			// do nothing
			return RT_EOK;
		}

		break;

	case SCENE_DISP_DATA:

		if (scene_isChanged) {
			scene_isChanged = RT_FALSE;

			// rebind key interrupt
			rt_pin_detach_irq(KEY1_EMBEDDED);
			rt_pin_attach_irq(KEY1_EMBEDDED, PIN_IRQ_MODE_RISING, key1_irq_DISP_DATA, RT_NULL);
			rt_pin_irq_enable(KEY1_EMBEDDED, PIN_IRQ_ENABLE);
			rt_pin_detach_irq(KEY2_EMBEDDED);
			rt_pin_attach_irq(KEY2_EMBEDDED, PIN_IRQ_MODE_RISING, key2_irq_DISP_DATA, RT_NULL);
			rt_pin_irq_enable(KEY2_EMBEDDED, PIN_IRQ_ENABLE);

			DISP_DATA_task();
			return RT_EOK;

		} else {
			DISP_DATA_curr_milli = rt_tick_get_millisecond();
			if (DISP_DATA_curr_milli - DISP_DATA_prev_milli >= DISP_DATA_DELAY) {
				DISP_DATA_prev_milli = DISP_DATA_curr_milli;
				DISP_DATA_updateData();
			}

			if (isSavingData == RT_TRUE) {
				isSavingData = RT_FALSE;
				sdcard_saveCurrData();
			}

			return RT_EOK;
		}

		break;

	case SCENE_TAKE_PHOTO:
		if (scene_isChanged) {
			scene_isChanged = RT_FALSE;

			// rebind key interrupt
			rt_pin_detach_irq(KEY1_EMBEDDED);
			rt_pin_attach_irq(KEY1_EMBEDDED, PIN_IRQ_MODE_RISING, key1_irq_TAKE_PHOTO, RT_NULL);
			rt_pin_irq_enable(KEY1_EMBEDDED, PIN_IRQ_ENABLE);
			rt_pin_detach_irq(KEY2_EMBEDDED);
			rt_pin_attach_irq(KEY2_EMBEDDED, PIN_IRQ_MODE_RISING, key2_irq_TAKE_PHOTO, RT_NULL);
			rt_pin_irq_enable(KEY2_EMBEDDED, PIN_IRQ_ENABLE);

			// ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
			return RT_EOK;

		} else {
			TAKE_PHOTO_task();
			return RT_EOK;
		}

		break;

	case SCENE_SD_CARD:
		if (scene_isChanged) {
			scene_isChanged = RT_FALSE;

			// rebind key interrupt
			rt_pin_detach_irq(KEY1_EMBEDDED);
			rt_pin_attach_irq(KEY1_EMBEDDED, PIN_IRQ_MODE_RISING, key1_irq_SD_CARD, RT_NULL);
			rt_pin_irq_enable(KEY1_EMBEDDED, PIN_IRQ_ENABLE);
			rt_pin_detach_irq(KEY2_EMBEDDED);
			rt_pin_attach_irq(KEY2_EMBEDDED, PIN_IRQ_MODE_RISING, key2_irq_SD_CARD, RT_NULL);
			rt_pin_irq_enable(KEY2_EMBEDDED, PIN_IRQ_ENABLE);

			SD_CARD_task();
			return RT_EOK;

		} else {
			// do nothing
			return RT_EOK;
		}

		break;

	case SCENE_SETTINGS:
		if (scene_isChanged) {
			scene_isChanged = RT_FALSE;

			// rebind key interrupt
			rt_pin_detach_irq(KEY1_EMBEDDED);
			rt_pin_attach_irq(KEY1_EMBEDDED, PIN_IRQ_MODE_RISING, key1_irq_SETTING, RT_NULL);
			rt_pin_irq_enable(KEY1_EMBEDDED, PIN_IRQ_ENABLE);
			rt_pin_detach_irq(KEY2_EMBEDDED);
			rt_pin_attach_irq(KEY2_EMBEDDED, PIN_IRQ_MODE_RISING, key2_irq_SETTING, RT_NULL);
			rt_pin_irq_enable(KEY2_EMBEDDED, PIN_IRQ_ENABLE);

			SETTINGS_task();
			return RT_EOK;

		} else {
			// do nothing
			return RT_EOK;
		}

		break;

	default:
		return -RT_ERROR;
		break;
	}
}

// store data to sd card
void sdcard_saveCurrData(void)
{
	led_embedded_color(LED_BLUE);
	ILI9341_Clear(0, LINE(13), LCD_X_LENGTH, 16);
	LCD_SetFont(&Font8x16);
	LCD_SetTextColor(CL_YELLOW);
	ILI9341_DispStringLine_EN(LINE(13), "         Saving current data...");

	char headingStr[32];
	char temperatureStr[32];
	char humidityStr[32];
	char baroStr[32];
	char uviStr[32];
	char luxStr[32];
	char pmStr[32];
	char soundStr[32];
	sprintf(headingStr, "Heading:\t%f\n", hmc5883l_heading);
	sprintf(temperatureStr, "Temperature:\t%f\n", bme280_rawData.temperature);
	sprintf(humidityStr, "Humidity:\t%f\n", bme280_rawData.humidity);
	sprintf(baroStr, "Baro:\t%f\n", bme280_rawData.pressure / 100);
	sprintf(uviStr, "UVI:\t%d\n", ltr390_rawData.uvi);
	sprintf(luxStr, "LUX:\t%f\n", ltr390_rawData.lux);
	sprintf(pmStr, "PM2.5:\t%f\n", gp2y1014au0f_rawData);
	sprintf(soundStr, "Sound:\t%f\n", lm386_rawData);
	char dataBlock[8 * 32];
	strcpy(dataBlock, headingStr);
	strcat(dataBlock, temperatureStr);
	strcat(dataBlock, humidityStr);
	strcat(dataBlock, baroStr);
	strcat(dataBlock, uviStr);
	strcat(dataBlock, luxStr);
	strcat(dataBlock, pmStr);
	strcat(dataBlock, soundStr);
	// LOG_D("%s", dataBlock);
	rt_uint32_t dataLen = rt_strlen(dataBlock);
	// for (rt_uint32_t i = dataLen; i < 8 * 32; i++) {
	// 	dataBlock[i] = ' ';
	// }
	// !!! important
	dataBlock[dataLen] = '\n';
	sdcard_appendNewData(dataBlock, dataLen + 1);

	led_embedded_color(LED_GREEN);
	LCD_SetTextColor(CL_GREEN);
	ILI9341_DispStringLine_EN(LINE(13), "     Saving current data succeeded!");
}

// scene tasks
void MENU_task(void)
{
	ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
	LCD_SetFont(&Font24x32);
	LCD_SetTextColor(CL_CYAN);
	ILI9341_DispString_EN(3 * 24 - 4, LINE(0), "WELCOME!");
	LCD_SetFont(&Font8x16);
	LCD_SetTextColor(CL_GREY1);
	ILI9341_DispString_EN(9 * 8, LINE(2), "K1: Select | K2: Enter");
	LCD_SetFont(&Font16x24);
	LCD_SetTextColor(CL_GREY3);
	ILI9341_DispStringLine_EN(LINE(3), "    Display Data");
	ILI9341_DispStringLine_EN(LINE(4), "    Take Photo");
	ILI9341_DispStringLine_EN(LINE(5), "    Eject SD Card");
	ILI9341_DispStringLine_EN(LINE(6), "    Settings");
	MENU_updateSelect();
}

void MENU_updateSelect(void)
{
	LCD_SetFont(&Font16x24);
	LCD_SetTextColor(CL_GREEN);
	ILI9341_Clear(0, LINE(3), 3 * 16, 4 * 24);
	ILI9341_DispStringLine_EN(LINE(selectScene + 2), " >>");
}

void DISP_DATA_task(void)
{
	ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
	LCD_SetFont(&Font24x32);
	LCD_SetTextColor(CL_CYAN);
	ILI9341_DispString_EN(3 * 24 - 8, LINE(0), "ENV DATA");
	LCD_SetFont(&Font8x16);
	LCD_SetTextColor(CL_GREY1);
	ILI9341_DispString_EN(9 * 8 - 8, LINE(2), "K1: Save to SD | K2: Back");
	LCD_SetTextColor(CL_GREY3);
	// title: 0 - 23 | data: 23 - 30 | unit: 30
	ILI9341_DispStringLine_EN(LINE(4U), "      Heading:       ");
	ILI9341_DispStringLine_EN(LINE(5U), "      Temperature:   ");
	ILI9341_DispStringLine_EN(LINE(6U), "      Humidity:      ");
	ILI9341_DispStringLine_EN(LINE(7U), "      Baro Pressure: ");
	ILI9341_DispStringLine_EN(LINE(8U), "      UV Index:      ");
	ILI9341_DispStringLine_EN(LINE(9U), "      Luminance:     ");
	ILI9341_DispStringLine_EN(LINE(10), "      PM2.5 Level:   ");
	ILI9341_DispStringLine_EN(LINE(11), "      Sound Level:   ");
	ILI9341_DispString_EN(28 * 8, LINE(4U), " *");
	ILI9341_DispString_EN(28 * 8, LINE(5U), " *C");
	ILI9341_DispString_EN(28 * 8, LINE(6U), " %");
	ILI9341_DispString_EN(28 * 8, LINE(7U), " hPa");
	ILI9341_DispString_EN(28 * 8, LINE(8U), " uvi");
	ILI9341_DispString_EN(28 * 8, LINE(9U), " lux");
	ILI9341_DispString_EN(28 * 8, LINE(10), " ug/m3");
	ILI9341_DispString_EN(28 * 8, LINE(11), " level");
	// update data
	DISP_DATA_curr_milli = rt_tick_get_millisecond();
	DISP_DATA_prev_milli == DISP_DATA_curr_milli;
	DISP_DATA_updateData();
}

void DISP_DATA_updateData(void)
{
	char headingStr[8];
	char temperatureStr[8];
	char humidityStr[8];
	char baroStr[8];
	char uviStr[8];
	char luxStr[8];
	char pmStr[8];
	char soundStr[8];
	sprintf(headingStr, "%-4.2f", hmc5883l_heading);
	sprintf(temperatureStr, "%-4.2f", bme280_rawData.temperature);
	sprintf(humidityStr, "%-4.2f", bme280_rawData.humidity);
	sprintf(baroStr, "%-4.2f", bme280_rawData.pressure / 100);
	sprintf(uviStr, "%-d", ltr390_rawData.uvi);
	sprintf(luxStr, "%-5.1f", ltr390_rawData.lux);
	sprintf(pmStr, "%-4.2f", gp2y1014au0f_rawData);
	sprintf(soundStr, "%-4.2f", lm386_rawData);
	LCD_SetFont(&Font8x16);
	LCD_SetTextColor(CL_WHITE);
	// title: 0 - 23 | data: 23 - 30 | unit: 30
	ILI9341_Clear(21 * 8, LINE(4), 8 * 8, 8 * 16);
	ILI9341_DispString_EN(21 * 8, LINE(4U), headingStr);
	ILI9341_DispString_EN(21 * 8, LINE(5U), temperatureStr);
	ILI9341_DispString_EN(21 * 8, LINE(6U), humidityStr);
	ILI9341_DispString_EN(21 * 8, LINE(7U), baroStr);
	ILI9341_DispString_EN(21 * 8, LINE(8U), uviStr);
	ILI9341_DispString_EN(21 * 8, LINE(9U), luxStr);
	ILI9341_DispString_EN(21 * 8, LINE(10), pmStr);
	ILI9341_DispString_EN(21 * 8, LINE(11), soundStr);
}

void TAKE_PHOTO_task(void)
{
	my_ov7725_test();
}

void SD_CARD_task(void)
{
	ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
	LCD_SetFont(&Font24x32);
	LCD_SetTextColor(CL_YELLOW);
	ILI9341_DispString_EN(0, LINE(0), "CONFIRM TO EJECT?");
	LCD_SetFont(&Font8x16);
	LCD_SetTextColor(CL_WHITE);
	ILI9341_DispString_EN(9 * 8, LINE(2), "K1: Select | K2: Enter");
}

void SETTINGS_task(void)
{
	ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
}

// key interrupts for different scenes
void key1_irq_MENU(void)
{
	select_isChanged = RT_TRUE;
	selectScene = (selectScene + 1) % 5;
	if (selectScene == SCENE_MENU) {
		selectScene = SCENE_DISP_DATA;
	}
}

void key2_irq_MENU(void)
{
	scene_isChanged = RT_TRUE;
	prevScene = currScene;
	currScene = selectScene;
}

void key1_irq_DISP_DATA(void)
{
	isSavingData = RT_TRUE;
}

void key2_irq_DISP_DATA(void)
{
	scene_isChanged = RT_TRUE;
	prevScene = currScene;
	currScene = SCENE_MENU;
}

void key1_irq_TAKE_PHOTO(void)
{
	// TODO write to SD card
}

void key2_irq_TAKE_PHOTO(void)
{
	scene_isChanged = RT_TRUE;
	prevScene = currScene;
	currScene = SCENE_MENU;
}

void key1_irq_SD_CARD(void)
{
	// release SD card
	if (sdcard_unmount() != RT_EOK) {
		isEjectSuccess = RT_FALSE;
	} else {
		isEjectSuccess = RT_TRUE;
	}
}

void key2_irq_SD_CARD(void)
{
	scene_isChanged = RT_TRUE;
	prevScene = currScene;
	currScene = SCENE_MENU;
}

void key1_irq_SETTING(void)
{
	// TODO
}

void key2_irq_SETTING(void)
{
	scene_isChanged = RT_TRUE;
	prevScene = currScene;
	currScene = SCENE_MENU;
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
			// LOG_D("x: %d", hmc5883l_rawData.x);
			// LOG_D("y: %d", hmc5883l_rawData.y);
			// LOG_D("z: %d", hmc5883l_rawData.z);
			const double PI = 3.14159265;
			hmc5883l_heading = atan2(hmc5883l_rawData.y, hmc5883l_rawData.x) * (180 / PI) + 180;
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

/* 系统时钟信息查看 */
// void clockinfo(void)
// {
// 	volatile uint32_t rcc_cr = RCC->CR; //读取RCC_CR寄存器值
// 	rt_kprintf("RCC_CR:%#X.\r\n", rcc_cr);

// 	/* System Clock source */
// 	if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_MSI) {
// 		rt_kprintf("SYSCKL source: MSI.\r\n");
// 	} else if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSI) {
// 		rt_kprintf("SYSCKL source: HSI.\r\n");
// 	} else if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSE) {
// 		rt_kprintf("SYSCKL source: HSE.\r\n");
// 	} else if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK) {
// 		rt_kprintf("SYSCKL source: PLL.\r\n");
// 	}

// 	/* PLL Clock source */
// 	if (__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSI) {
// 		/* PLL source is HSI oscillator */
// 		rt_kprintf("PLL source:HSI oscillator.\r\n");
// 	} else if (__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSE) {
// 		/* PLL source is HSE bypass oscillator */
// 		rt_kprintf("PLL source:HSE bypass oscillator.\r\n");
// 	} else if (__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_MSI) {
// 		/* PLL source is MSI oscillator */
// 		rt_kprintf("PLL source:MSI oscillator.\r\n");
// 	}

// 	/* Clock Frequency */
// 	rt_kprintf("SystemClockFreq:%lu.\r\n", HAL_RCC_GetSysClockFreq());
// 	rt_kprintf("HCLKFreq:%lu.\r\n", HAL_RCC_GetHCLKFreq());
// 	rt_kprintf("PCLK1Freq:%lu.\r\n", HAL_RCC_GetPCLK1Freq());
// 	rt_kprintf("PCLK2Freq:%lu.\r\n", HAL_RCC_GetPCLK2Freq());

// 	/* Peripheral Clock Frequency */ //各个外设时钟频率
// 	rt_kprintf("RTC\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_RTC)); //RTC
// 	rt_kprintf("ADC\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_ADC)); //ADC
// 	rt_kprintf("I2C1\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I2C1)); //I2C1
// 	rt_kprintf("I2C2\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I2C2)); //I2C2
// 	rt_kprintf("I2C3\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I2C3)); //I2C3
// 	rt_kprintf("LPTIM1\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_LPTIM1)); //LPTIM1
// 	rt_kprintf("LPTIM2\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_LPTIM2)); //LPTIM2
// 	rt_kprintf("LPUART1\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_LPUART1)); //LPUART1
// 	rt_kprintf("RGN\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_RNG)); //RNG
// 	rt_kprintf("SAI1\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SAI1)); //SAI1
// 	rt_kprintf("SDMMC1\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC1)); //SDMMC1
// 	rt_kprintf("SWPMI1\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SWPMI1)); //SWPMI1
// 	rt_kprintf("USART1\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_USART1)); //USART1
// 	rt_kprintf("USART2\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_USART2)); //USART2
// 	rt_kprintf("USART3\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_USART3)); //USART3
// 	rt_kprintf("UART4\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_UART4)); //UART4
// 	rt_kprintf("UART5\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_UART5)); //UART5
// 	rt_kprintf("USB\tClockFreq:%lu.\r\n", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_USB)); //USB
// }
// /* 导出到 msh 命令列表中 */
// MSH_CMD_EXPORT(clockinfo, display system clock info);
