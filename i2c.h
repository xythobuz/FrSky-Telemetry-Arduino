/*
 * FrSky Telemetry Display for Arduino
 *
 * I2C / TWI Interface
 * Copied from the OLED library in the HobbyKing shop comments:
 * http://www.hobbyking.com/hobbyking/store/uploads/121771368X1172162X54.zip
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */
#ifndef __I2C_H__
#define __I2C_H__

#include <Arduino.h>

#define OLED_address 0x3c
#define INTERNAL_I2C_PULLUPS
#define I2C_SPEED 100000L

void i2c_init(void);
void i2c_rep_start(uint8_t address);
void i2c_stop(void);
void i2c_write(uint8_t data);
uint8_t i2c_read(uint8_t ack);
uint8_t i2c_readAck(void);
uint8_t i2c_readNak(void);
void waitTransmissionI2C(void);
size_t i2c_read_to_buf(uint8_t add, void *buf, size_t size);
size_t i2c_read_reg_to_buf(uint8_t add, uint8_t reg, void *buf, size_t size);
void i2c_writeReg(uint8_t add, uint8_t reg, uint8_t val);

void swap_endianness(void *buf, size_t size);

#endif

