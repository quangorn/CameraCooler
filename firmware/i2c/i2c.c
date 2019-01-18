#include "i2c.h"
#include <avr/io.h>
#include <util/delay.h>

//=========== Definitions ============
#define I2C_START        0x08	//Start command transmitted
#define I2C_START_REPEAT 0x10	//Repeated start command transmitted
#define I2C_SLA_W_ACK    0x18	//Slave address in write mode transmitted, ACK received
#define I2C_TRANSMIT_ACK 0x28	//Data byte transmitted, ACK received
#define I2C_SLA_R_ACK    0x40	//Slave address in read mode transmitted, ACK received
#define I2C_RECEIVE_ACK  0x50	//Data byte received, ACK sent
#define I2C_RECEIVE_NACK 0x58	//Data byte received, NACK sent

int8_t sendImpl(int8_t additionalTWCR_Flags);

void i2cInit() {
	//setting transfer speed 100 kHz
	TWBR = 0x48;
}

int8_t i2cStart() {
	int8_t status = sendImpl(1 << TWSTA);
	return status == I2C_START || status == I2C_START_REPEAT;
}

void i2cStop() {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

int8_t i2cWriteSlaveAddr(uint8_t addr, int8_t isRead) {
	TWDR = addr << 1 | isRead;
	int8_t status = sendImpl(0);
	return (isRead && status == I2C_SLA_R_ACK) || status == I2C_SLA_W_ACK;
}

int8_t i2cWriteDataByte(uint8_t data) {
	TWDR = data;
	return sendImpl(0) == I2C_TRANSMIT_ACK;
}

int8_t i2cRead(uint8_t *dataPtr, uint16_t len) {
	if (!len) {
		return 0;
	}
	for (uint16_t i = 0; i < len - 1; i++) {
		if (sendImpl(1 << TWEA) != I2C_RECEIVE_ACK) {
			return 0;
		}
		dataPtr[i] = TWDR;
	}

	if (sendImpl(0) != I2C_RECEIVE_NACK) {
		return 0;
	}
	dataPtr[len - 1] = TWDR;
	return 1;
}

int8_t sendImpl(int8_t additionalTWCR_Flags) {
	TWCR = (1 << TWINT) | (1 << TWEN) | additionalTWCR_Flags;
	while (!(TWCR & (1 << TWINT)));
	return TWSR & 0xF8;
}
