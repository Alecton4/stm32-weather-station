#include "i2c_LTR390.h"

/**
 * This function reads the value of registers for ltr390
 *
 * @param dev the pointer of device driver structure
 * @param reg the register for ltr390
 * @param buf read data pointer
 * @param len number of register
 *
 * @return the reading status, RT_EOK represents reading the value of registers successfully.
 */
static rt_err_t read_regs(struct ltr390_device_struct *dev, rt_uint8_t reg, rt_uint8_t *buf, rt_uint8_t len)
{
	if (dev->bus->type == RT_Device_Class_I2CBUS) {
		struct rt_i2c_msg msgs[2];

		msgs[0].addr = dev->i2c_addr; /* Slave address */
		msgs[0].flags = RT_I2C_WR; /* Write flag */
		msgs[0].buf = &reg; /* Slave register address */
		msgs[0].len = 1; /* Number of bytes sent */

		msgs[1].addr = dev->i2c_addr; /* Slave address */
		msgs[1].flags = RT_I2C_RD; /* Read flag */
		msgs[1].buf = buf; /* Read data pointer */
		msgs[1].len = len; /* Number of bytes read */

		if (rt_i2c_transfer((struct rt_i2c_bus_device *)dev->bus, msgs, 2) == 2) {
			return RT_EOK;
		} else {
			return -RT_ERROR;
		}
	} else {
		return -RT_ERROR;
	}
}

/**
 * This function writes the value of registers for ltr390
 *
 * @param dev the pointer of device driver structure
 * @param reg the register for ltr390
 * @param buf read data pointer
 * @param len number of register
 *
 * @return the writing status, RT_EOK represents reading the value of registers successfully.
 */
static rt_err_t write_regs(struct ltr390_device_struct *dev, rt_uint8_t reg, rt_uint8_t *buf, rt_uint8_t len)
{
	if (dev->bus->type == RT_Device_Class_I2CBUS) {
		struct rt_i2c_msg msgs[2];

		msgs[0].addr = dev->i2c_addr; /* Slave address */
		msgs[0].flags = RT_I2C_WR | RT_I2C_NO_STOP; /* Write flag */
		msgs[0].buf = &reg; /* Slave register address */
		msgs[0].len = 1; /* Number of bytes sent */

		msgs[1].addr = dev->i2c_addr; /* Slave address */
		msgs[1].flags = RT_I2C_WR | RT_I2C_NO_START; /* Read flag */
		msgs[1].buf = buf; /* Read data pointer */
		msgs[1].len = len; /* Number of bytes read */

		if (rt_i2c_transfer((struct rt_i2c_bus_device *)dev->bus, msgs, 2) == 2) {
			return RT_EOK;
		} else {
			return -RT_ERROR;
		}
	} else {
		return -RT_ERROR;
	}
}

/**
 * This function writes the value of the register for ltr390
 *
 * @param dev the pointer of device driver structure
 * @param reg the register for ltr390
 * @param data value to write
 *
 * @return the writing status, RT_EOK represents writing the value of the register successfully.
 */
static rt_err_t write_reg(struct ltr390_device_struct *dev, rt_uint8_t reg, rt_uint8_t data)
{
	if (dev->bus->type == RT_Device_Class_I2CBUS) {
		rt_uint8_t buffer[2];
		buffer[0] = reg;
		buffer[1] = data;

		struct rt_i2c_msg msg;
		msg.addr = dev->i2c_addr; /* slave address */
		msg.flags = RT_I2C_WR; /* write flag */
		msg.buf = buffer; /* Send data pointer */
		msg.len = 2;

		if (rt_i2c_transfer((struct rt_i2c_bus_device *)dev->bus, &msg, 1) == 1) {
			return RT_EOK;
		} else {
			return -RT_ERROR;
		}
	} else {
		return -RT_ERROR;
	}
}

/**
 * This function initialize the ltr390 device.
 *
 * @param i2c_bus_name the name of transfer device
 * @param addr the i2c device address for i2c communication
 *
 * @return the pointer of device driver structure, RT_NULL represents initialization failed.
 */
struct ltr390_device_struct *my_ltr390_init(const char *i2c_bus_name, rt_uint8_t addr)
{
	struct ltr390_device_struct *ltr390 = RT_NULL;
	ltr390 = rt_calloc(1, sizeof(struct ltr390_device_struct));
	if (ltr390 == RT_NULL) {
		LOG_E("Can't allocate memory for ltr390 device on '%s' ", i2c_bus_name);
		return RT_NULL;
	}

	RT_ASSERT(i2c_bus_name);
	ltr390->bus = rt_device_find(i2c_bus_name);
	if (ltr390->bus == RT_NULL) {
		LOG_E("Can't find device:'%s'", i2c_bus_name);
		rt_free(ltr390);
		return RT_NULL;
	}

	if (ltr390->bus->type == RT_Device_Class_I2CBUS) {
		ltr390->i2c_addr = addr;
	} else {
		LOG_E("Unsupported device:'%s'!", i2c_bus_name);
		rt_free(ltr390);
		return RT_NULL;
	}

	// sensor configuration
	if ((write_reg(ltr390, LTR390_REGISTER_MEAS_RATE, LTR390_RESOLUTION_16BIT_TIME25MS | LTR390_RATE_25MS) != RT_EOK) ||
	    (write_reg(ltr390, LTR390_REGISTER_GAIN, LTR390_GAIN_3) != RT_EOK)) {
		LOG_E("ltr390 cannot be configured!");
		rt_free(ltr390);
		return RT_NULL;
	}

	LOG_I("Device init succeed!");
	return ltr390;
}

/**
 * This function releases memory
 *
 * @param ltr390 the pointer of device driver structure
 */
void my_ltr390_destroy(struct ltr390_device_struct *ltr390)
{
	RT_ASSERT(ltr390);
	rt_free(ltr390);
}

/**
 * This function gets the raw data of the mag
 *
 * @param ltr390 the pointer of device driver structure
 * @param data the pointer of received data
 *
 * @return the reading status, RT_EOK represents reading the data successfully.
 */
rt_err_t my_ltr390_get_data(struct ltr390_device_struct *ltr390, struct ltr390_data_struct *data)
{
	if (ltr390 == RT_NULL || data == RT_NULL) {
		return -RT_ERROR;
	} else {
		rt_uint8_t buffer[3];
		rt_uint8_t status;

		// measure ambient light
		if (write_reg(ltr390, LTR390_REGISTER_MAIN_CTRL, 0x02) &&
		    write_reg(ltr390, LTR390_REGISTER_INT_CFG, 0x14) != RT_EOK) {
			LOG_E("ltr390 cannot be configured!");
			return -RT_ERROR;
		}
		// !!! remember to change parameters if sensor configuration is changed
		rt_thread_mdelay(25);
		status = 0x00;
		if (read_regs(ltr390, LTR390_REGISTER_MAIN_STATUS, &status, 1) != RT_EOK) {
			LOG_E("ltr390 cannot be read!");
			return -RT_ERROR;
		}
		status &= 0x08;
		if (status == 0x08) {
			if (read_regs(ltr390, LTR390_REGISTER_ALSDATA, buffer, 3) != RT_EOK) {
				LOG_E("ltr390 cannot be read!");
				return -RT_ERROR;
			}
			rt_uint32_t alsData = (rt_uint32_t)(buffer[2] << 16 | buffer[1] << 8 | buffer[0]);
			// !!! remember to change parameters if sensor configuration is changed
			data->lux = (0.6 * alsData) / (3 * 0.25);
		}

		// measure uv
		if (write_reg(ltr390, LTR390_REGISTER_MAIN_CTRL, 0x0A) &&
		    write_reg(ltr390, LTR390_REGISTER_INT_CFG, 0x34) != RT_EOK) {
			LOG_E("ltr390 cannot be configured!");
			return -RT_ERROR;
		}
		// !!! remember to change parameters if sensor configuration is changed
		rt_thread_mdelay(25);
		status = 0x00;
		if (read_regs(ltr390, LTR390_REGISTER_MAIN_STATUS, &status, 1) != RT_EOK) {
			LOG_E("ltr390 cannot be read!");
			return -RT_ERROR;
		}
		status &= 0x08;
		if (status == 0x08) {
			if (read_regs(ltr390, LTR390_REGISTER_UVSDATA, buffer, 3) != RT_EOK) {
				LOG_E("ltr390 cannot be read!");
				return -RT_ERROR;
			}
			rt_uint32_t uvsData = (rt_uint32_t)(buffer[2] << 16 | buffer[1] << 8 | buffer[0]);
			// REF https://github.com/adafruit/Adafruit_CircuitPython_LTR390/blob/main/adafruit_ltr390.py
			// !!! remember to change parameters if sensor configuration is changed
			double sensitivity = 1400.00 / 18 * 3 / pow(2, (20 - 16));
			data->uvi = uvsData / sensitivity;
		}

		return RT_EOK;
	}
}
