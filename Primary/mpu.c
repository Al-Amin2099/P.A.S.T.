#include "mpu.h"
#include "i2c.h"

#define REG_PWR_MGMT_1   0x6B
#define REG_WHO_AM_I     0x75
#define REG_GYRO_CONFIG  0x1B
#define REG_GYRO_XOUT_H  0x43

/* +/-250 dps range => sensitivity 131 LSB per deg/s (datasheet value) */
#define GYRO_SENS 131.0f

static float _gx_offset = 0, _gy_offset = 0, _gz_offset = 0;
static float _gx = 0, _gy = 0, _gz = 0;

uint8_t mpu_init(void)
{
    uint8_t who = 0;
    if (i2c_read_reg(MPU_ADDR, REG_WHO_AM_I, &who)) return 0;

    /* Wake the device up (clear the sleep bit) */
    if (i2c_write_reg(MPU_ADDR, REG_PWR_MGMT_1, 0x00)) return 0;

    /* +/-250 dps full scale */
    if (i2c_write_reg(MPU_ADDR, REG_GYRO_CONFIG, 0x00)) return 0;

    return 1;
}

void mpu_calibrate_gyro(uint16_t samples)
{
    int32_t sx = 0, sy = 0, sz = 0;
    uint8_t buf[6];

    for (uint16_t i = 0; i < samples; i++)
    {
        i2c_read_regs(MPU_ADDR, REG_GYRO_XOUT_H, buf, 6);
        sx += (int16_t)((buf[0] << 8) | buf[1]);
        sy += (int16_t)((buf[2] << 8) | buf[3]);
        sz += (int16_t)((buf[4] << 8) | buf[5]);
    }

    _gx_offset = (float)sx / samples;
    _gy_offset = (float)sy / samples;
    _gz_offset = (float)sz / samples;
}

uint8_t mpu_update(void)
{
    uint8_t buf[6];
    if (i2c_read_regs(MPU_ADDR, REG_GYRO_XOUT_H, buf, 6)) return 0;

    int16_t rawX = (int16_t)((buf[0] << 8) | buf[1]);
    int16_t rawY = (int16_t)((buf[2] << 8) | buf[3]);
    int16_t rawZ = (int16_t)((buf[4] << 8) | buf[5]);

    _gx = ((float)rawX - _gx_offset) / GYRO_SENS;
    _gy = ((float)rawY - _gy_offset) / GYRO_SENS;
    _gz = ((float)rawZ - _gz_offset) / GYRO_SENS;

    return 1;
}

float mpu_get_gyro_x(void) { return _gx; }
float mpu_get_gyro_y(void) { return _gy; }
float mpu_get_gyro_z(void) { return _gz; }
