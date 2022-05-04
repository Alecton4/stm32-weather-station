#ifndef MY_MAIN_H
#define MY_MAIN_H

// #ifdef __cplusplus
// extern "C"
// {
// #endif

/* Includes ------------------------------------------------------------------*/
#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "stm32f1xx_hal.h"
// for outputing debugging info
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "led_embedded.h"
#include "key_embedded.h"
#include "i2c_HMC5883L.h"
#include "i2c_BME280.h"
#include "i2c_LTR390.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

// #ifdef __cplusplus
// }
// #endif

#endif /* __MAIN_H */
