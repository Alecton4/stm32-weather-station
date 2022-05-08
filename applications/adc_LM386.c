#include "adc_LM386.h"
// for outputing debugging info
#define DBG_TAG "LM386"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

// struct lm386_device_struct *my_lm386_init(const char *adc_dev_name, rt_uint8_t adc_dev_channel)
// {
// 	struct lm386_device_struct *lm386 = RT_NULL;
// 	lm386 = rt_calloc(1, sizeof(struct lm386_device_struct));
// 	if (lm386 == RT_NULL) {
// 		LOG_E("Can't allocate memory for lm386 device on '%s' ", adc_dev_name);
// 		return RT_NULL;
// 	}

// 	ASSERT(adc_dev_name);
// 	lm386->adc = (rt_adc_device_t)rt_device_find(adc_dev_name);
// 	if (lm386->adc == RT_NULL) {
// 		LOG_E("Can't find %s device!\n", adc_dev_name);
// 		rt_free(lm386);
// 		return RT_NULL;
// 	} else {
// 		lm386->adc_channel = adc_dev_channel;
// 	}

// 	return lm386;
// }

// void my_lm386_destroy(struct lm386_device_struct *lm386)
// {
// 	RT_ASSERT(lm386);
// 	rt_free(lm386);
// }

// rt_err_t my_lm386_get_data(struct lm386_device_struct *lm386, rt_uint32_t *data)
// {
// 	if (rt_adc_enable(lm386->adc, lm386->adc_channel) != RT_EOK) {
// 		LOG_E("Cannot enable %s device on %d channel!\n", lm386->adc, lm386->adc_channel);
// 		rt_free(lm386);
// 		return RT_NULL;
// 	}

// 	rt_uint32_t value;
// 	value = rt_adc_read(lm386->adc, lm386->adc_channel);
// 	LOG_D("the value is :%d", value);
// 	*data = value * LM386_REFER_VOLTAGE / LM386_CONVERT_BITS;
// 	LOG_D("the voltage is :%d.%02d", *data / 100, *data % 100);

// 	if (rt_adc_disable(lm386->adc, lm386->adc_channel) != RT_EOK) {
// 		LOG_E("Cannot disable %s device on %d channel!\n", lm386->adc, lm386->adc_channel);
// 		rt_free(lm386);
// 		return RT_NULL;
// 	}

// 	return RT_EOK;
// }

static ADC_HandleTypeDef hadc1;

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void my_MX_ADC1_Init(void)
{
	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */
	/** Common config
  */
	hadc1.Instance = ADC1;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	if (HAL_ADC_Init(&hadc1) != HAL_OK) {
		Error_Handler();
	}
	/** Configure Regular Channel
  */
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */
}

void my_lm386_init(void)
{
	my_MX_ADC1_Init();
	// HAL_ADC_Start_IT(&hadc1);
}

double my_lm386_get_data()
{
	rt_uint32_t value;
	double voltage;

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 100);

	value = HAL_ADC_GetValue(&hadc1);
	LOG_D("the value is: %d", value);
	voltage = value / 4096.0 * 3.3;
	LOG_D("the voltage is: %0.2f", voltage);

	HAL_ADC_Stop(&hadc1);
	return voltage;
}
