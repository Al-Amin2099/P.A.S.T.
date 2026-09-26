#include "i2c.h"
#include <avr/io.h>
#include <util/twi.h>

#define I2C_SCL_FREQ 100000UL /* 100kHz standard mode */

void i2c_init(void)
{
    TWSR = 0x00; /* prescaler = 1 */
    TWBR = (uint8_t)(((F_CPU / I2C_SCL_FREQ) - 16) / 2);
    TWCR = (1 << TWEN);
}

uint8_t i2c_start(uint8_t address_rw)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    uint8_t status = TW_STATUS;
    if (status != TW_START && status != TW_REP_START) return 1;

    TWDR = address_rw;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    status = TW_STATUS;
    if (status != TW_MT_SLA_ACK && status != TW_MR_SLA_ACK) return 1;

    return 0;
}

void i2c_stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while (TWCR & (1 << TWSTO));
}

uint8_t i2c_write(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    return (TW_STATUS != TW_MT_DATA_ACK);
}

uint8_t i2c_read_ack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

uint8_t i2c_read_nack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

uint8_t i2c_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t data)
{
    if (i2c_start((uint8_t)((dev_addr << 1) | TW_WRITE))) { i2c_stop(); return 1; }
    if (i2c_write(reg))  { i2c_stop(); return 1; }
    if (i2c_write(data)) { i2c_stop(); return 1; }
    i2c_stop();
    return 0;
}

uint8_t i2c_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *data)
{
    return i2c_read_regs(dev_addr, reg, data, 1);
}

uint8_t i2c_read_regs(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
    if (i2c_start((uint8_t)((dev_addr << 1) | TW_WRITE))) { i2c_stop(); return 1; }
    if (i2c_write(reg)) { i2c_stop(); return 1; }
    if (i2c_start((uint8_t)((dev_addr << 1) | TW_READ))) { i2c_stop(); return 1; }

    for (uint8_t i = 0; i < len; i++)
    {
        buf[i] = (i == len - 1) ? i2c_read_nack() : i2c_read_ack();
    }
    i2c_stop();
    return 0;
}
