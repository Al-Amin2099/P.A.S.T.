#ifndef MPU_H
#define MPU_H

#include <stdint.h>

/* NOTE: same default address as the RTC in this project (0x68).
 * A real MPU9250/9255 has an AD0 pin: tie it high to move the sensor
 * to 0x69 and resolve the conflict with the RTC on the same bus.
 * Verify your wiring before flashing. */
#define MPU_ADDR 0x68

/* Only gyro X/Y are used by main.c's loop (matches the original .ino,
 * which never reads magnetometer data despite calling calibrateMag()).
 * Accelerometer and magnetometer support are intentionally omitted. */

uint8_t mpu_init(void);              /* returns 1 on success, 0 on failure */
void mpu_calibrate_gyro(uint16_t samples);
uint8_t mpu_update(void);            /* reads a new sample; 1 on success */

float mpu_get_gyro_x(void);
float mpu_get_gyro_y(void);
float mpu_get_gyro_z(void);

#endif
