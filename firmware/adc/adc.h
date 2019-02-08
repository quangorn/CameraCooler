#ifndef CAMERACOOLER_ADC_H
#define CAMERACOOLER_ADC_H

#include <stdint.h>

void adcInit();
void adcStartConversion();
int16_t adcGetTemp();

#endif //CAMERACOOLER_ADC_H
