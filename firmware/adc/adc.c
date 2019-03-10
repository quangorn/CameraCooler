#include "adc.h"
#include <math.h>
#include <avr/io.h>
#include <common/definitions.h>

#define ADC_SAMPLES_SIZE 8 //can't be more than 64 or uint16_t can overflow on sum

uint16_t adcSamples[ADC_SAMPLES_SIZE];
uint8_t adcCurrentSampleIdx = 0;

void adcInit() {
	ADCSRA = (1 << ADEN)	//Enable ADC
	         | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  //Set prescaler = 128 (16MHz/128 = 125kHz)
	ADMUX |= (1 << REFS0) | (1 << MUX0);	//AVcc reference voltage, Select ADC1 (PC1)
	ADCSRA |= (1 << ADSC);  //Start A2D Conversions
}

void adcReadNextSample() {
	adcSamples[adcCurrentSampleIdx++] = ADC;
	if (adcCurrentSampleIdx >= ADC_SAMPLES_SIZE) {
		adcCurrentSampleIdx = 0;
	}
	ADCSRA |= (1 << ADSC);  //Start A2D Conversions
}

int16_t adcGetTemp(struct Settings* settings) {
	uint16_t adcSum = 0;
	for (int8_t i = 0; i < ADC_SAMPLES_SIZE; i++) {
		adcSum += adcSamples[i];
	}
	double rThermistor = settings->balanceResistor * ((MAX_ADC_VALUE * ADC_SAMPLES_SIZE / adcSum) - 1);
	double temp = (settings->betaCoeff * ROOM_TEMP) /
	              (settings->betaCoeff + (ROOM_TEMP * log(rThermistor / settings->thermistorValue))) - 273.15;
	return (int16_t)(temp * 100);
}