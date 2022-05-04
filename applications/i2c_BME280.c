#include "i2c_BME280.h"

static struct rt_i2c_bus_device *i2c_bus_toBeUsed;

/*
 * Return control or wait,
 * for a period amount of milliseconds
 */
static void my_delay_ms(uint32_t period)
{
	rt_thread_mdelay(period);
}

/*
 * int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
 * The parameter dev_id can be used as a variable to store the I2C address of the device
 * Data on the bus should be like
 * |------------+---------------------|
 * | I2C action | Data                |
 * |------------+---------------------|
 * | Start      | -                   |
 * | Write      | (reg_addr)          |
 * | Stop       | -                   |
 * | Start      | -                   |
 * | Read       | (reg_data[0])       |
 * | Read       | (....)              |
 * | Read       | (reg_data[len - 1]) |
 * | Stop       | -                   |
 * |------------+---------------------|
 */
static rt_err_t read_regs(rt_uint8_t addr, rt_uint8_t reg, rt_uint8_t *data, rt_uint16_t len)
{
	// rt_uint8_t tmp = reg;
	struct rt_i2c_msg msgs[2];

	msgs[0].addr = addr; /* Slave address */
	msgs[0].flags = RT_I2C_WR; /* Write flag */
	msgs[0].buf = &reg; /* Slave register address */
	msgs[0].len = 1; /* Number of bytes sent */

	msgs[1].addr = addr; /* Slave address */
	msgs[1].flags = RT_I2C_RD; /* Read flag */
	msgs[1].buf = data; /* Read data pointer */
	msgs[1].len = len; /* Number of bytes read */

	if (rt_i2c_transfer(i2c_bus_toBeUsed, msgs, 2) != 2) {
		return -RT_ERROR;
	} else {
		return RT_EOK;
	}
}

/*
 * int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
 * The parameter dev_id can be used as a variable to store the I2C address of the device
 * Data on the bus should be like
 * |------------+---------------------|
 * | I2C action | Data                |
 * |------------+---------------------|
 * | Start      | -                   |
 * | Write      | (reg_addr)          |
 * | Write      | (reg_data[0])       |
 * | Write      | (....)              |
 * | Write      | (reg_data[len - 1]) |
 * | Stop       | -                   |
 * |------------+---------------------|
 */
static rt_err_t write_regs(rt_uint8_t addr, rt_uint8_t reg, rt_uint8_t *data, rt_uint16_t len)
{
	// rt_uint8_t tmp = reg;
	struct rt_i2c_msg msgs[2];

	msgs[0].addr = addr; /* Slave address */
	msgs[0].flags = RT_I2C_WR | RT_I2C_NO_STOP; /* Write flag */
	msgs[0].buf = &reg; /* Slave register address */
	msgs[0].len = 1; /* Number of bytes sent */

	msgs[1].addr = addr; /* Slave address */
	msgs[1].flags = RT_I2C_WR | RT_I2C_NO_START; /* Write flag */
	msgs[1].buf = data; /* Write data pointer */
	msgs[1].len = len; /* Number of bytes write */

	if (rt_i2c_transfer(i2c_bus_toBeUsed, msgs, 2) != 2) {
		return -RT_ERROR;
	} else {
		return RT_EOK;
	}
}

struct bme280_device_struct *my_bme280_init(const char *i2c_bus_name, rt_uint8_t addr)
{
	struct bme280_device_struct *bme280 = RT_NULL;
	bme280 = rt_calloc(1, sizeof(struct bme280_device_struct));
	if (bme280 == RT_NULL) {
		LOG_E("Can't allocate memory for bme280 device on '%s' ", i2c_bus_name);
		return RT_NULL;
	}

	RT_ASSERT(i2c_bus_name);
	bme280->bus = rt_device_find(i2c_bus_name);
	if (bme280->bus == RT_NULL) {
		LOG_E("Can't find device:'%s'", i2c_bus_name);
		rt_free(bme280);
		return RT_NULL;
	}

	if (bme280->bus->type == RT_Device_Class_I2CBUS) {
		bme280->i2c_addr = addr;
	} else {
		LOG_E("Unsupported device:'%s'!", i2c_bus_name);
		rt_free(bme280);
		return RT_NULL;
	}

	i2c_bus_toBeUsed = (struct rt_i2c_bus_device *)(bme280->bus);
	bme280->dev.dev_id = addr;
	bme280->dev.intf = BME280_I2C_INTF;
	bme280->dev.read = read_regs;
	bme280->dev.write = write_regs;
	bme280->dev.delay_ms = my_delay_ms;
	if (bme280_init(&(bme280->dev)) != BME280_OK) {
		LOG_E("bme280 init failed");
		rt_free(bme280);
		return RT_NULL;
	}

	if (bme280_crc_selftest(&(bme280->dev)) != BME280_OK) {
		LOG_E("bme280 test failed");
		rt_free(bme280);
		return RT_NULL;
	}

	/* Recommended mode of operation: weather monitoring */
	bme280->dev.settings.osr_t = BME280_OVERSAMPLING_4X;
	bme280->dev.settings.osr_h = BME280_OVERSAMPLING_4X;
	bme280->dev.settings.osr_p = BME280_OVERSAMPLING_4X;
	bme280->dev.settings.filter = BME280_FILTER_COEFF_OFF;
	rt_uint8_t settings_sel = BME280_OSR_TEMP_SEL | BME280_OSR_PRESS_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

	if (bme280_set_sensor_settings(settings_sel, &(bme280->dev)) != BME280_OK) {
		LOG_E("bme280 config failed");
		rt_free(bme280);
		return RT_NULL;
	}

	if (bme280_set_sensor_mode(BME280_FORCED_MODE, &(bme280->dev)) != BME280_OK) {
		LOG_E("bme280 set forced_mode failed");
		rt_free(bme280);
		return RT_NULL;
	}

	LOG_I("Device init succeed!");
	return bme280;
}

/**
 * This function releases memory
 *
 * @param bme280 the pointer of device driver structure
 */
void my_bme280_destroy(struct bme280_device_struct *bme280)
{
	RT_ASSERT(bme280);
	rt_free(bme280);
}

rt_err_t my_bme280_get_data(struct bme280_device_struct *bme280, struct bme280_data *data)
{
	if (bme280 == RT_NULL || data == RT_NULL) {
		return -RT_ERROR;
	} else {
		i2c_bus_toBeUsed = (struct rt_i2c_bus_device *)(bme280->bus);

		if (bme280_set_sensor_mode(BME280_FORCED_MODE, &(bme280->dev)) != BME280_OK) {
			LOG_E("bme280 set forced_mode failed");
			return -RT_ERROR;
		}
		// for debugging
		// rt_uint8_t mode;
		// bme280_get_sensor_mode(&mode, &(bme280->dev));
		// LOG_D("mode set to: %x", mode);

		if (bme280_get_sensor_data(BME280_ALL, data, &(bme280->dev)) != BME280_OK) {
			LOG_E("bme280 read data failed");
			return -RT_ERROR;
		}

		return RT_EOK;
	}
}
