#ifndef BME280_USER_H_
#define BME280_USER_H_

#include <stdint.h>
#include "bme280_defs.h"

int8_t bmeInit();
int8_t bmeStartInNormalMode();
int8_t bmeGetCurrentData(struct bme280_data* dataPtr);

#endif /* BME280_USER_H_ */