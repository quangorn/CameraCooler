#ifndef CAMERACOOLER_SETTINGS_H
#define CAMERACOOLER_SETTINGS_H

#include <stdint.h>

struct Settings {
	int16_t targetTemp;
	int16_t dewPointUnsafeZone;
	uint16_t balanceResistor;
	uint16_t thermistorValue;
	uint16_t betaCoeff;
	//PID-controller parameters
	int16_t pFactor;
	int16_t iFactor;
	int16_t dFactor;
};

#endif //CAMERACOOLER_SETTINGS_H
