#ifndef CAMERACOOLER_COOLER_H
#define CAMERACOOLER_COOLER_H

#include <stdint.h>
#include <stdbool.h>

void coolerInit();
void coolerSetPower(uint8_t power);
void coolerSetState(bool state);

#endif //CAMERACOOLER_COOLER_H
