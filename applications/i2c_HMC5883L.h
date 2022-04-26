#ifndef MY_HMC5883L_H
#define MY_HMC5883L_H

#include "main.h"

#define HMC5883L_I2C_BUS "i2c1"
#define HMC5883L_ADDR 0x1E
#define HMC5883L_CONFIG_A 0x70
#define HMC5883L_CONFIG_B 0xA0
#define HMC5883L_MODE 0x01

/* Supported configuration items */
enum hmc5883l_cmd {
	HMC5883L_SAMPLE_NUM = 0, /* number of samples averaged (1 to 8) per measurement output */
	HMC5883L_DOR = 1, /* Output data rate(only in continuous mode) */
	HMC5883L_RANGE = 2, /* Mag full scale range */
	HMC5883L_OPER_MODE = 3 /* Continuous or single measurement mode */
};

enum hmc5883l_sample_num {
	HMC5883L_SAMPLES_AVER_1 = 0,
	HMC5883L_SAMPLES_AVER_2 = 1,
	HMC5883L_SAMPLES_AVER_4 = 2,
	HMC5883L_SAMPLES_AVER_8 = 3
};

enum hmc5883l_dor {
	HMC5883L_DOR_0_75Hz = 0,
	HMC5883L_DOR_1_5Hz = 1,
	HMC5883L_DOR_3_0Hz = 2,
	HMC5883L_DOR_7_5Hz = 3,
	HMC5883L_DOR_15Hz = 4,
	HMC5883L_DOR_30Hz = 5,
	HMC5883L_DOR_75Hz = 6,
	HMC5883L_DOR_Reserved = 7
};

enum hmc5883l_range {
	HMC5883L_RANGE_0_88Ga = 0, // ¡À0.88G Gain:1370
	HMC5883L_RANGE_1_3Ga = 1, // ¡À1.3G  Gain:1090
	HMC5883L_RANGE_1_9Ga = 2, // ¡À1.9G  Gain:820
	HMC5883L_RANGE_2_5Ga = 3, // ¡À2.5G  Gain:660
	HMC5883L_RANGE_4_0Ga = 4, // ¡À4.0G  Gain:440
	HMC5883L_RANGE_4_7Ga = 5, // ¡À4.7G  Gain:390
	HMC5883L_RANGE_5_6Ga = 6, // ¡À5.6G  Gain:330
	HMC5883L_RANGE_8_1Ga = 7 // ¡À8.1G  Gain:230
};

enum hmc5883l_oper_mode { HMC5883L_CONTINUOUS = 0, HMC5883L_SINGLE = 1 };

struct hmc5883l_data_struct {
	rt_int16_t x;
	rt_int16_t y;
	rt_int16_t z;
};

struct hmc5883l_config_struct {
	rt_uint16_t range;
	rt_uint16_t odr;
	rt_uint8_t meas_mode;
	rt_uint8_t oprt_mode;
};

/* hmc5883 device structure */
struct hmc5883l_device_struct {
	rt_device_t bus;
	rt_uint8_t id;
	rt_uint8_t i2c_addr;
	struct hmc5883l_config_struct config;
	float range_scale;
};

struct hmc5883l_device_struct *hmc5883l_init(const char *i2c_bus, rt_uint8_t addr);
void hmc5883l_destroy(struct hmc5883l_device_struct *dev);
rt_err_t hmc5883l_get_param(struct hmc5883l_device_struct *dev, enum hmc5883l_cmd cmd, rt_uint8_t *param);
rt_err_t hmc5883l_set_param(struct hmc5883l_device_struct *dev, enum hmc5883l_cmd cmd, rt_uint8_t param);
rt_err_t hmc5883l_get_data(struct hmc5883l_device_struct *dev, struct hmc5883l_data_struct *data);

#endif