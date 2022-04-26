#include "i2c_HMC5883L.h"

/**
 * This function reads the value of registers for hmc5883l
 *
 * @param dev the pointer of device driver structure
 * @param reg the register for hmc5883l
 * @param len number of register
 * @param buf read data pointer
 *
 * @return the reading status, RT_EOK represents reading the value of registers successfully.
 */
static rt_err_t read_regs(struct hmc5883l_device_struct *dev, rt_uint8_t reg, rt_uint8_t len, rt_uint8_t *buf)
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
			return RT_ERROR;
		}
	} else {
		return RT_ERROR;
	}
}

/**
 * This function reads multi-bit value of registers for hmc5883l
 *
 * @param dev the pointer of device driver structure
 * @param reg the register for hmc5883l
 * @param start_bit the start position of the register
 * @param len number of bits to write
 * @param data read data pointer
 *
 * @return the reading status, RT_EOK reprensents  reading multi-bit value of registers successfully.
 */
static rt_err_t read_bits(struct hmc5883l_device_struct *dev, rt_uint8_t reg, rt_uint8_t start_bit, rt_uint8_t len,
			  rt_uint8_t *data)
{
	rt_uint8_t byte, mask;
	rt_err_t res;

	res = read_regs(dev, reg, 1, &byte);
	if (res != RT_EOK) {
		return res;
	}

	mask = ((1 << len) - 1) << (start_bit - len + 1);
	byte &= mask;
	byte >>= (start_bit - len + 1);
	*data = byte;

	return RT_EOK;
}

/**
 * This function writes the value of the register for hmc5883l
 *
 * @param dev the pointer of device driver structure
 * @param reg the register for hmc5883l
 * @param data value to write
 *
 * @return the writing status, RT_EOK represents writing the value of the register successfully.
 */
static rt_err_t write_reg(struct hmc5883l_device_struct *dev, rt_uint8_t reg, rt_uint8_t data)
{
	if (dev->bus->type == RT_Device_Class_I2CBUS) {
		rt_uint8_t tmp[2];
		tmp[0] = reg;
		tmp[1] = data;

		struct rt_i2c_msg msg;
		msg.addr = dev->i2c_addr; /* slave address */
		msg.flags = RT_I2C_WR; /* write flag */
		msg.buf = tmp; /* Send data pointer */
		msg.len = 2;

		if (rt_i2c_transfer((struct rt_i2c_bus_device *)dev->bus, &msg, 1) == 1) {
			return RT_EOK;
		} else {
			return RT_ERROR;
		}
	} else {
		return RT_ERROR;
	}
}

/**
 * This function writes multi-bit value of registers for hmc5883l
 *
 * @param dev the pointer of device driver structure
 * @param reg the register for hmc5883l
 * @param start_bit the start position of the register
 * @param len number of bits to write
 * @param data value to write
 *
 * @return the writing status, RT_EOK represents writing multi-bit value of registers successfully.
 */
static rt_err_t write_bits(struct hmc5883l_device_struct *dev, rt_uint8_t reg, rt_uint8_t start_bit, rt_uint8_t len,
			   rt_uint8_t data)
{
	rt_uint8_t byte, mask;
	rt_err_t res;

	res = read_regs(dev, reg, 1, &byte);
	if (res != RT_EOK) {
		return res;
	}

	mask = ((1 << len) - 1) << (start_bit - len + 1);
	data <<= (start_bit - len + 1); // shift data into correct position
	data &= mask; // zero all non-important bits in data
	byte &= ~(mask); // zero all important bits in existing byte
	byte |= data; // combine data with existing byte

	return write_reg(dev, reg, byte);
}

rt_err_t self_test(struct hmc5883l_device_struct *dev)
{
	rt_uint8_t data[3] = { 0 };
	read_regs(dev, 0x0A, 3, data);

	if (data[0] == 0x48 && data[1] == 0x34 && data[2] == 0x33) {
		return RT_EOK;
	} else {
		return RT_ERROR;
	}
}

/**
 * This function gets hmc5883l parameters.
 *
 * @param dev the pointer of device driver structure
 * @param cmd Configuration item
 * @param param read data pointer
 *
 * @return the reading status, RT_EOK represents reading the data successfully.
 */
rt_err_t hmc5883l_get_param(struct hmc5883l_device_struct *dev, enum hmc5883l_cmd cmd, rt_uint8_t *param)
{
	rt_uint8_t data = 0;
	rt_err_t res = RT_EOK;

	RT_ASSERT(dev);

	switch (cmd) {
	case HMC5883L_SAMPLE_NUM:
		res = read_bits(dev, 0x00, 5, 2, &data);
		*param = data;
		break;
	case HMC5883L_DOR:
		res = read_bits(dev, 0x00, 2, 3, &data);
		*param = data;
		break;
	case HMC5883L_RANGE:
		res = read_bits(dev, 0x01, 5, 3, &data);
		*param = data;
		break;
	case HMC5883L_OPER_MODE:
		res = read_bits(dev, 0x02, 0, 2, &data);
		*param = data;
		break;
	}

	return res;
}

/**
 * This function set hmc5883l parameters.
 *
 * @param dev the pointer of device driver structure
 * @param cmd Configuration item
 * @param param Configuration item parameter
 *
 * @return the setting status, RT_EOK reprensents  setting the parameter successfully.
 */
rt_err_t hmc5883l_set_param(struct hmc5883l_device_struct *dev, enum hmc5883l_cmd cmd, rt_uint8_t param)
{
	rt_err_t res = RT_EOK;

	RT_ASSERT(dev);

	switch (cmd) {
	case HMC5883L_SAMPLE_NUM:
		res = write_bits(dev, 0x00, 5, 2, param);
		dev->config.meas_mode = param;
		break;
	case HMC5883L_DOR:
		res = write_bits(dev, 0x00, 2, 3, param);
		dev->config.odr = param;
		break;
	case HMC5883L_RANGE:
		res = write_bits(dev, 0x01, 5, 3, param);
		dev->config.range = param;
		break;
	case HMC5883L_OPER_MODE:
		res = write_bits(dev, 0x02, 1, 2, param);
		dev->config.oper_mode = param;
		break;
	}

	return res;
}

/**
 * This function initialize the hmc5883l device.
 *
 * @param i2c_bus the name of transfer device
 * @param addr the i2c device address for i2c communication
 *
 * @return the pointer of device driver structure, RT_NULL reprensents  initialization failed.
 */
struct hmc5883l_device_struct *hmc5883l_init(const char *i2c_bus, rt_uint8_t addr)
{
	struct hmc5883l_device_struct *dev = RT_NULL;
	dev = rt_calloc(1, sizeof(struct hmc5883l_device_struct));
	if (dev == RT_NULL) {
		LOG_E("Can't allocate memory for hmc5883l device on '%s' ", i2c_bus);
		return RT_NULL;
	}

	RT_ASSERT(i2c_bus);
	dev->bus = rt_device_find(i2c_bus);
	if (dev->bus == RT_NULL) {
		LOG_E("Can't find device:'%s'", i2c_bus);
		rt_free(dev);
		return RT_NULL;
	}

	if (dev->bus->type == RT_Device_Class_I2CBUS) {
		dev->i2c_addr = addr;
	} else {
		LOG_E("Unsupported device:'%s'!", i2c_bus);
		rt_free(dev);
		return RT_NULL;
	}

	if (self_test(dev) != RT_EOK) {
		LOG_E("hmc5883l test self fail!");
		rt_free(dev);
		return RT_NULL;
	}

	// hmc5883l_set_param(dev, HMC5883L_RANGE, HMC5883L_RANGE_1_9Ga);
	// hmc5883l_set_param(dev, HMC5883L_DOR, HMC5883L_DOR_15Hz);
	// hmc5883l_set_param(dev, HMC5883L_RANGE, HMC5883L_RANGE_4_7Ga);
	if ((write_reg(dev, 0x00, 0x70) != RT_EOK) || (write_reg(dev, 0x01, 0xA0) != RT_EOK)) {
		LOG_E("hmc5883l cannot be configured!");
		rt_free(dev);
		return RT_NULL;
	}

	LOG_I("Device init succeed!");
	return dev;
}

/**
 * This function releases memory
 *
 * @param dev the pointer of device driver structure
 */
void hmc5883l_destroy(struct hmc5883l_device_struct *dev)
{
	RT_ASSERT(dev);
	rt_free(dev);
}

/**
 * This function gets the raw data of the mag
 *
 * @param dev the pointer of device driver structure
 * @param data the pointer of 3axes structure for receive data
 *
 * @return the reading status, RT_EOK represents reading the data successfully.
 */
rt_err_t hmc5883l_get_data(struct hmc5883l_device_struct *dev, struct hmc5883l_data_struct *data)
{
	if (dev == RT_NULL || data == RT_NULL) {
		return RT_ERROR;
	} else {
		if (write_reg(dev, 0x02, 0x01) != RT_EOK) {
			return RT_ERROR;
		}
		// for debugging
		// rt_uint8_t configs[3];
		// if (read_regs(dev, 0x00, 3, configs) != RT_EOK) {
		// 	return RT_ERROR;
		// }
		// LOG_E("0x00: 0x%02X", configs[0]);
		// LOG_E("0x01: 0x%02X", configs[1]);
		// LOG_E("0x02: 0x%02X", configs[2]);

		rt_uint8_t buffer[10];
		if (read_regs(dev, 0x03, 6, buffer) != RT_EOK) {
			return RT_ERROR;
		}
		data->x = (rt_int16_t)(((buffer[0]) << 8) | buffer[1]);
		data->y = (rt_int16_t)(((buffer[4]) << 8) | buffer[5]);
		data->z = (rt_int16_t)(((buffer[2]) << 8) | buffer[3]);
		// for debugging
		// LOG_E("0x03: 0x%02X", buffer[0]);
		// LOG_E("0x04: 0x%02X", buffer[1]);
		// LOG_E("0x05: 0x%02X", buffer[2]);
		// LOG_E("0x06: 0x%02X", buffer[3]);
		// LOG_E("0x07: 0x%02X", buffer[4]);
		// LOG_E("0x08: 0x%02X", buffer[5]);
		// LOG_D("x: %d", (rt_int16_t)(((buffer[0]) << 8) | buffer[1]));
		// LOG_D("y: %d", (rt_int16_t)(((buffer[4]) << 8) | buffer[5]));
		// LOG_D("z: %d", (rt_int16_t)(((buffer[2]) << 8) | buffer[3]));
		// LOG_D("x: %d", data->x);
		// LOG_D("y: %d", data->y);
		// LOG_D("z: %d", data->z);

		return RT_EOK;
	}
}
