#ifndef CAMERACOOLER_SETTINGS_H
#define CAMERACOOLER_SETTINGS_H

#include <stdint.h>

struct Settings {
	int16_t targetTemp;
	int16_t dewPointUnsafeZone;
	uint16_t balanceResistor;
};

#endif //CAMERACOOLER_SETTINGS_H
