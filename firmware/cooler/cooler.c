#include "cooler.h"
#include <avr/io.h>

void coolerInit() {
	//PD6 (OC0A) is now an output
	DDRD |= 1 << DDD6;
	//PB1 (OC1A) is now an output
	DDRB |= 1 << DDB1;

	//set none-inverting mode and 8bit fast PWM Mode, enable PWM on channel A
	TCCR0A |= (1 << WGM01) | (1 << WGM00) | (1 << COM0A1);
	TCCR1A |= (1 << WGM10) | (1 << COM1A1);

	//set prescaler to 1024
	TCCR0B |= (1 << CS00) | (1 << CS02);
	//set prescaler to 8
	TCCR1B |= (1 << WGM12) | (1 << CS11);
}

void coolerSetPower(uint8_t power) {
	OCR0A = power;
	OCR1A = power;
}
