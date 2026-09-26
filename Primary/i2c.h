#ifndef I2C_H
#define I2C_H

#include <stdint.h>

void i2c_init(void);

/* Low-level primitives, exposed in case you need custom transactions */
uint8_t i2c_start(uint8_t address_rw); /* address already shifted, RW bit set */
void i2c_stop(void);
uint8_t i2c_write(uint8_t data);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);

/* Convenience helpers used by rtc.c / mpu.c. dev_addr is the 7-bit address
 * (unshifted). Return 0 on success, nonzero on I2C error. */
uint8_t i2c_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t data);
uint8_t i2c_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *data);
uint8_t i2c_read_regs(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len);

#endif
