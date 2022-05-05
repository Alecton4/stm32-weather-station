#ifndef MY_LTR390_H
#define MY_LTR390_H

#include "main.h"

#define LTR390_I2C_BUS "i2c1"
#define LTR390_ADDR 0x53

// registers
#define LTR390_REGISTER_MAIN_CTRL 0x00 // Main control register
#define LTR390_REGISTER_MEAS_RATE 0x04 // Resolution and data rate
#define LTR390_REGISTER_GAIN 0x05 // ALS and UVS gain range
#define LTR390_REGISTER_PART_ID 0x06 // Part id/revision register
#define LTR390_REGISTER_MAIN_STATUS 0x07 // Main status register
#define LTR390_REGISTER_ALSDATA 0x0D // ALS data lowest byte, 3 byte
#define LTR390_REGISTER_UVSDATA 0x10 // UVS data lowest byte, 3 byte
#define LTR390_REGISTER_INT_CFG 0x19 // Interrupt configuration
#define LTR390_REGISTER_INT_PST 0x1A // Interrupt persistance config
#define LTR390_REGISTER_THRESH_UP 0x21 // Upper threshold, low byte, 3 byte
#define LTR390_REGISTER_THRESH_LOW 0x24 // Lower threshold, low byte, 3 byte

// ALS/UVS measurement resolution
#define LTR390_RESOLUTION_20BIT_TIME400MS 0X00
#define LTR390_RESOLUTION_19BIT_TIME200MS 0X10
#define LTR390_RESOLUTION_18BIT_TIME100MS 0X20 //default
#define LTR390_RESOLUTION_17BIT_TIME50MS 0x30
#define LTR390_RESOLUTION_16BIT_TIME25MS 0x40
#define LTR390_RESOLUTION_13BIT_TIME12_5MS 0x50

// ALS/UVS measurement rate
#define LTR390_RATE_25MS 0x00
#define LTR390_RATE_50MS 0x01
#define LTR390_RATE_100MS 0x02 //default
#define LTR390_RATE_200MS 0x03
#define LTR390_RATE_500MS 0x04
#define LTR390_RATE_1000MS 0x05
#define LTR390_RATE_2000MS 0x06

//measurement Gain Range.
#define LTR390_GAIN_1 0x00
#define LTR390_GAIN_3 0x01 // default
#define LTR390_GAIN_6 0x02
#define LTR390_GAIN_9 0x03
#define LTR390_GAIN_18 0x04

struct ltr390_data_struct {
	double lux;
	rt_uint8_t uvi;
};

struct ltr390_config_struct {
	rt_uint8_t sample_num;
	rt_uint16_t dor;
	rt_uint16_t range;
	rt_uint8_t oper_mode;
};

/* ltr390 device structure */
struct ltr390_device_struct {
	rt_device_t bus;
	rt_uint8_t i2c_addr;
	struct ltr390_config_struct config;
};

struct ltr390_device_struct *my_ltr390_init(const char *i2c_bus_name, rt_uint8_t addr);
void my_ltr390_destroy(struct ltr390_device_struct *ltr390);
rt_err_t my_ltr390_get_data(struct ltr390_device_struct *ltr390, struct ltr390_data_struct *data);

#endif