#ifndef MY_LM386_H
#define MY_LM386_H

#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>

#define LM386_ADC_DEV_NAME "adc1" /* ADC 设备名称 */
#define LM386_ADC_DEV_CHANNEL 5 /* ADC 通道 */
#define LM386_REFER_VOLTAGE 330 /* 参考电压 3.3V,数据精度乘以100保留2位小数*/
#define LM386_CONVERT_BITS (1 << 12) /* 转换位数为12位 */

// struct lm386_device_struct {
// 	rt_adc_device_t adc;
// 	rt_uint8_t adc_channel;
// };

// struct lm386_device_struct *my_lm386_init(const char *adc_dev_name, rt_uint8_t adc_device_channel);
// void my_lm386_destroy(struct lm386_device_struct *lm386);
// rt_err_t my_lm386_get_data(struct lm386_device_struct *adc_lm386, rt_uint32_t *data);

void my_lm386_init(void);
double my_lm386_get_data();

#endif
