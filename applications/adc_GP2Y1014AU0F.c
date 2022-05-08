#include "adc_GP2Y1014AU0F.h"
// for outputing debugging info
#define DBG_TAG "GP2Y1014AU0F"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

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
	sConfig.Channel = ADC_CHANNEL_5;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */
}

void my_gp2y1014au0f_init(void)
{
	my_MX_ADC1_Init();
	// HAL_ADC_Start_IT(&hadc1);
	rt_pin_mode(GP2Y1014AU0F_ILED_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(GP2Y1014AU0F_ILED_PIN, PIN_LOW);
}

static rt_uint32_t my_filter(rt_uint32_t data)
{
	static rt_uint32_t flag_first = 0;
	static rt_uint32_t buffer[10];
	static rt_uint32_t sum = 0;
	const rt_uint8_t BUFFER_LENGTH = 10;
	rt_uint8_t i;

	if (flag_first == 0) {
		flag_first = 1;
		for (i = 0, sum = 0; i < BUFFER_LENGTH; i++) {
			buffer[i] = data;
			sum += buffer[i];
		}
		return data;
	} else {
		sum -= buffer[0];
		for (i = 0; i < (BUFFER_LENGTH - 1); i++) {
			buffer[i] = buffer[i + 1];
		}
		buffer[9] = data;
		sum += buffer[9];

		i = sum / 10.0;
		return i;
	}
}

double my_gp2y1014au0f_get_data()
{
	rt_uint32_t value;
	double voltage;
	double density;

	rt_pin_write(GP2Y1014AU0F_ILED_PIN, PIN_HIGH);
	rt_hw_us_delay(300); // 传感器内部红外二极管在开启之后0.28ms，输出波形才达到稳定

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 100);
	value = HAL_ADC_GetValue(&hadc1);

	// rt_hw_us_delay(40); //整个脉冲持续时间为320μs。因此，我们还需再等待40μs
	rt_pin_write(GP2Y1014AU0F_ILED_PIN, PIN_LOW);
	// rt_hw_us_delay(9680); //需要脉宽比0.32ms/10ms的PWM信号驱动传感器中的LED

	value = my_filter(value);
	LOG_D("the value is: %d", value);
	voltage = (value / 4096.0) * VOLTAGE_SYS * 11;
	LOG_D("the voltage is: %f", voltage);
	if (voltage >= VOLTAGE_NO_DUST) {
		voltage -= VOLTAGE_NO_DUST;
		density = voltage * COV_RATIO;
	} else {
		density = 0;
	}
	LOG_D("The current dust concentration is: %4.2fug/m3", density);

	HAL_ADC_Stop(&hadc1);
	return density;
}

double my_gp2y1014au0f_get_data_avg(rt_uint32_t sample_num)
{
	double avg_density = 0;
	for (rt_uint32_t i = 0; i < sample_num; i++) {
		avg_density += my_gp2y1014au0f_get_data();
	}
	return avg_density / sample_num;
}
