#ifndef CAMERACOOLER_RUNTIME_INFO_H
#define CAMERACOOLER_RUNTIME_INFO_H

#include <stdint.h>

struct RuntimeInfo {
	int16_t chipTemp;
	int16_t caseTemp;
	uint16_t caseHumidity;
	int16_t dewPoint;
	int16_t targetTemp;
	uint8_t coolerPower;
};

#endif //CAMERACOOLER_RUNTIME_INFO_H
