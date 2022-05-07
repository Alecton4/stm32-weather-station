#ifndef MY_BME280_H
#define MY_BME280_H

#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "Bosch_BME280.h"
#include "Bosch_BME280_selftest.h"

#define BME280_I2C_BUS "i2c1"
#define BME280_ADDR BME280_I2C_ADDR_SEC

/* hmc5883 device structure */
struct bme280_device_struct {
	rt_device_t bus;
	rt_uint8_t i2c_addr;
	struct bme280_dev dev;
};

struct bme280_device_struct *my_bme280_init(const char *i2c_bus_name, rt_uint8_t addr);
void my_bme280_destroy(struct bme280_device_struct *bme280);
rt_err_t my_bme280_get_data(struct bme280_device_struct *bme280, struct bme280_data *data);

#endif
