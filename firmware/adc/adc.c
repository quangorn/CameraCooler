#include "adc.h"
#include <avr/io.h>

void adcInit() {
	ADCSRA = (1 << ADEN)	//Enable ADC
	         | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  //Set prescaler = 128 (16MHz/128 = 125kHz)
	ADMUX |= (1 << REFS0) | (1 << MUX0);	//AVcc reference voltage, Select ADC1 (PC1)
}

void adcStartConversion() {
	ADCSRA |= (1 << ADSC);  //Start A2D Conversions
}

int16_t adcGetTemp() {
	return ADC;
}