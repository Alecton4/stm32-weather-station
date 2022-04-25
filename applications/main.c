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
struct hmc5833l_config_struct *hmc5883l_config = RT_NULL;
struct hmc5883l_data_struct *hmc5883l_data = RT_NULL;
// rt_uint8_t CRA = 0x70;
// rt_uint8_t CRB = 0xA0;
// rt_uint8_t MR = 0x01;

// -------- HAL VARIABLES --------
// I2C_HandleTypeDef hi2c2;

// -------- FUNCTION DECLARATION --------

// -------- HAL FUNCTION DECLARATION --------
// static void MX_I2C2_Init(void);

int main(void)
{
	int count = 1;

	// MX_I2C2_Init();
	led_embedded_init();
	key_embedded_init();
	hmc5883l = hmc5883l_init(HMC5883L_I2C_BUS_NAME, HMC5883L_ADDR);
	// HAL_I2C_Mem_Write(&hi2c2, HMC5883L_ADDR << 1, 0x00, 1, &CRA, 1, 100);
	// HAL_I2C_Mem_Write(&hi2c2, HMC5883L_ADDR << 1, 0x01, 1, &CRB, 1, 100);

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

		hmc5883l_data = rt_calloc(1, sizeof(struct hmc5883l_data_struct));
		hmc5883l_get_data(hmc5883l, hmc5883l_data);
		LOG_D("x: %d", hmc5883l_data->x);
		LOG_D("y: %d", hmc5883l_data->y);
		LOG_D("z: %d", hmc5883l_data->z);
		rt_free(hmc5883l_data);

		// rt_uint8_t raw_data_x[2] = { 0 };
		// rt_uint8_t raw_data_y[2] = { 0 };
		// rt_uint8_t raw_data_z[2] = { 0 };
		// rt_int16_t data_x = 0;
		// rt_int16_t data_y = 0;
		// rt_int16_t data_z = 0;
		// HAL_I2C_Mem_Write(&hi2c2, HMC5883L_ADDR << 1, 0x02, 1, &MR, 1, 100);
		// HAL_Delay(10);
		// HAL_I2C_Mem_Read(&hi2c2, HMC5883L_ADDR << 1, 0x03, 1, raw_data_x, 2, 100);
		// HAL_I2C_Mem_Read(&hi2c2, HMC5883L_ADDR << 1, 0x07, 1, raw_data_y, 2, 100);
		// HAL_I2C_Mem_Read(&hi2c2, HMC5883L_ADDR << 1, 0x05, 1, raw_data_z, 2, 100);
		// data_x = (int16_t)(((raw_data_x[0]) << 8) | raw_data_x[1]);
		// data_y = (int16_t)(((raw_data_y[0]) << 8) | raw_data_y[1]);
		// data_z = (int16_t)(((raw_data_z[0]) << 8) | raw_data_z[1]);
		// LOG_D("x: %d", data_x);
		// LOG_D("y: %d", data_y);
		// LOG_D("z: %d", data_z);

		rt_thread_mdelay(1000);
	}

	return RT_EOK;
}

// /**
//   * @brief I2C2 Initialization Function
//   * @param None
//   * @retval None
//   */
// static void MX_I2C2_Init(void)
// {
// 	/* USER CODE BEGIN I2C2_Init 0 */

// 	/* USER CODE END I2C2_Init 0 */

// 	/* USER CODE BEGIN I2C2_Init 1 */

// 	/* USER CODE END I2C2_Init 1 */
// 	hi2c2.Instance = I2C2;
// 	hi2c2.Init.ClockSpeed = 100000;
// 	hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
// 	hi2c2.Init.OwnAddress1 = 0;
// 	hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
// 	hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
// 	hi2c2.Init.OwnAddress2 = 0;
// 	hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
// 	hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
// 	if (HAL_I2C_Init(&hi2c2) != HAL_OK) {
// 		Error_Handler();
// 	}
// 	/* USER CODE BEGIN I2C2_Init 2 */

// 	/* USER CODE END I2C2_Init 2 */
// }
