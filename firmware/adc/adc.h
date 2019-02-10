#ifndef CAMERACOOLER_ADC_H
#define CAMERACOOLER_ADC_H

#include <stdint.h>
#include <common/settings.h>

void adcInit();
void adcReadNextSample();
int16_t adcGetTemp(struct Settings* settings);

#endif //CAMERACOOLER_ADC_H
