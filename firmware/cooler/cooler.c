#include "cooler.h"
#include <avr/io.h>

#define MIN_FAN_SPEED 50 //20%

void enableFan() {
	TCCR2A |= 1 << COM2A1;
}

void disableFan() {
	TCCR2A &= ~(1 << COM2A1);
}

void coolerSetState(bool state) {
	if (state) {
		PORTB &= ~(1 << PORTB2);
	} else {
		PORTB |= 1 << PORTB2;
	}
}

void coolerInit() {
	//PB1(OC1A), PB2, PB3(OC2A) are now an output
	DDRB |= (1 << DDB1) | (1 << DDB2) | (1 << DDB3);

	//set none-inverting mode and 8bit fast PWM Mode
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	//set inverting mode and 8bit fast PWM Mode, enable PWM on channel A
	TCCR1A |= (1 << WGM10) | (1 << COM1A0) | (1 << COM1A1);

	//set prescaler to 1024
	TCCR2B |= (1 << CS20) | (1 << CS21) | (1 << CS22);
	//set prescaler to 8
	TCCR1B |= (1 << WGM12) | (1 << CS11);
}

void coolerSetPower(uint8_t power) {
	if (power >= MIN_FAN_SPEED) {
		OCR2A = power;
		enableFan();
	} else {
		disableFan();
	}

	OCR1A = power;
}
