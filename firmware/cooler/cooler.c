#include "cooler.h"
#include <avr/io.h>

#define MIN_FAN_SPEED 50 //20%

void enableFan() {
	TCCR0A |= 1 << COM0A1;
}

void disableFan() {
	TCCR0A &= ~(1 << COM0A1);
}

void coolerInit() {
	//PD6 (OC0A) is now an output
	DDRD |= 1 << DDD6;
	//PB1 (OC1A) is now an output
	DDRB |= 1 << DDB1;

	//set none-inverting mode and 8bit fast PWM Mode
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	//set none-inverting mode and 8bit fast PWM Mode, enable PWM on channel A
	TCCR1A |= (1 << WGM10) | (1 << COM1A1);

	//set prescaler to 1024
	TCCR0B |= (1 << CS00) | (1 << CS02);
	//set prescaler to 8
	TCCR1B |= (1 << WGM12) | (1 << CS11);

	//cooler power is inverted
	OCR1A = 0xFF;
}

void coolerSetPower(uint8_t power) {
	if (power >= MIN_FAN_SPEED) {
		OCR0A = power;
		enableFan();
	} else {
		disableFan();
	}

	//cooler power is inverted
	OCR1A = 0xFFu - power;
}
