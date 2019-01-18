#include "bme280_user.h"
#include "bme280.h"
#include "../i2c/i2c.h"
#include <util/delay.h>

//============ Callbacks ============

int8_t bme280ReadCallback(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len) {
	int8_t ret = BME280_E_COMM_FAIL;
	if (i2cStart() && 
			i2cWriteSlaveAddr(dev_id, 0) &&
			i2cWriteDataByte(reg_addr) &&
			i2cStart() &&
			i2cWriteSlaveAddr(dev_id, 1) &&
			i2cRead(data, len)) {
		ret = BME280_OK;
	}
	i2cStop();
	return ret;
}

int8_t bme280WriteCallback(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len) {
	if (!len) {
		return BME280_E_INVALID_LEN;
	}
	int8_t ret = BME280_E_COMM_FAIL;
	if (i2cStart() &&
			i2cWriteSlaveAddr(dev_id, 0) &&
			i2cWriteDataByte(reg_addr)) {
		uint16_t i = 0;
		for (; i < len; i++) {
			if (!i2cWriteDataByte(data[i])) {
				break;
			}
		}
		if (i == len) {
			ret = BME280_OK;
		}
	}
	i2cStop();
	return ret;
}

void bme280DelayCallback(uint32_t period) {
	for (uint32_t i = 0; i < period; i++) {
		_delay_ms(1);
	}
}


//============ Public functions ============

struct bme280_dev dev;

int8_t bmeInit() {
	dev.dev_id = BME280_I2C_ADDR_PRIM;
	dev.intf = BME280_I2C_INTF;
	dev.read = bme280ReadCallback;
	dev.write = bme280WriteCallback;
	dev.delay_ms = bme280DelayCallback;

	return bme280_init(&dev);
}

int8_t bmeStartInNormalMode(struct bme280_data* dataPtr) {
	uint8_t settings_sel;

	/* Recommended mode of operation: Indoor navigation */
	dev.settings.osr_h = BME280_OVERSAMPLING_1X;
	dev.settings.osr_p = BME280_OVERSAMPLING_16X;
	dev.settings.osr_t = BME280_OVERSAMPLING_2X;
	dev.settings.filter = BME280_FILTER_COEFF_16;
	dev.settings.standby_time = BME280_STANDBY_TIME_1000_MS;

	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	settings_sel |= BME280_FILTER_SEL;

	int8_t result = bme280_set_sensor_settings(settings_sel, &dev);
	if (result == BME280_OK) {
		result = bme280_set_sensor_mode(BME280_NORMAL_MODE, &dev);
	}
	
	return result;
}

int8_t bmeGetCurrentData(struct bme280_data* dataPtr) {
	return bme280_get_sensor_data(BME280_ALL, dataPtr, &dev);
}

