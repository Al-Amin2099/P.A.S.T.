#ifndef RTC_H
#define RTC_H

#include <stdint.h>

/* NOTE: same default address as the MPU9250 in this project (0x68).
 * Verify on real hardware — see comment in mpu.h. */
#define RTC_ADDR 0x68

/* Assumes a DS1307/DS3231-compatible register layout:
 * 0x00 sec, 0x01 min, 0x02 hour (24h mode), 0x03 day-of-week,
 * 0x04 date, 0x05 month, 0x06 year (BCD, offset from 2000).
 * If your RTC module uses a different chip, these registers will
 * need to change. */

void rtc_set(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow,
             uint8_t day, uint8_t month, uint16_t year);
void rtc_refresh(void);

uint8_t rtc_second(void);
uint8_t rtc_minute(void);
uint8_t rtc_hour(void);
uint8_t rtc_day(void);
uint8_t rtc_month(void);
uint16_t rtc_year(void);
uint8_t rtc_dow(void);

#endif
