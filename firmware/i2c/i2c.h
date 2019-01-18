#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

void i2cInit();

int8_t i2cStart();
void i2cStop();

int8_t i2cWriteSlaveAddr(uint8_t addr, int8_t isRead);
int8_t i2cWriteDataByte(uint8_t data);
int8_t i2cRead(uint8_t *dataPtr, uint16_t len);

#endif /* I2C_H_ */