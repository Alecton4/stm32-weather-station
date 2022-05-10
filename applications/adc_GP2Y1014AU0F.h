#ifndef MY_GP2Y1014AU0F_H
#define MY_GP2Y1014AU0F_H

#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>

#define GP2Y1014AU0F_ADC_DEV_NAME "adc1" /* ADC 设备名称 */
#define GP2Y1014AU0F_ADC_DEV_CHANNEL 5 /* ADC 通道 */
#define GP2Y1014AU0F_REFER_VOLTAGE 330 /* 参考电压 3.3V,数据精度乘以100保留2位小数*/
#define GP2Y1014AU0F_CONVERT_BITS (1 << 12) /* 转换位数为12位 */

#define COV_RATIO (100 / 0.5) //ug/m^3 / mv
#define VOLTAGE_NO_DUST 0.4
#define VOLTAGE_SYS 3.3

// struct gp2y1014au0f_device_struct {
// 	rt_adc_device_t adc;
// 	rt_uint8_t adc_channel;
// };

// struct gp2y1014au0f_device_struct *my_gp2y1014au0f_init(const char *adc_dev_name, rt_uint8_t adc_device_channel);
// void my_gp2y1014au0f_destroy(struct gp2y1014au0f_device_struct *gp2y1014au0f);
// rt_err_t my_gp2y1014au0f_get_data(struct gp2y1014au0f_device_struct *adc_gp2y1014au0f, rt_uint32_t *data);

void my_gp2y1014au0f_init(void);
double my_gp2y1014au0f_get_data();
double my_gp2y1014au0f_get_data_avg(rt_uint32_t sample_num);

#endif
