#include "rtc.h"
#include "i2c.h"

static uint8_t _sec, _min, _hour, _day, _month, _dow;
static uint16_t _year;

static uint8_t bcd2dec(uint8_t b) { return (uint8_t)(((b >> 4) * 10) + (b & 0x0F)); }
static uint8_t dec2bcd(uint8_t d) { return (uint8_t)(((d / 10) << 4) | (d % 10)); }

void rtc_set(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow,
             uint8_t day, uint8_t month, uint16_t year)
{
    uint8_t buf[7];
    buf[0] = dec2bcd(sec);
    buf[1] = dec2bcd(min);
    buf[2] = dec2bcd(hour);
    buf[3] = dec2bcd(dow);
    buf[4] = dec2bcd(day);
    buf[5] = dec2bcd(month);
    buf[6] = dec2bcd((uint8_t)(year - 2000));

    for (uint8_t i = 0; i < 7; i++)
    {
        i2c_write_reg(RTC_ADDR, i, buf[i]);
    }
}

void rtc_refresh(void)
{
    uint8_t buf[7];
    i2c_read_regs(RTC_ADDR, 0x00, buf, 7);

    _sec   = bcd2dec(buf[0] & 0x7F);
    _min   = bcd2dec(buf[1] & 0x7F);
    _hour  = bcd2dec(buf[2] & 0x3F); /* assumes 24h mode */
    _dow   = bcd2dec(buf[3] & 0x07);
    _day   = bcd2dec(buf[4] & 0x3F);
    _month = bcd2dec(buf[5] & 0x1F);
    _year  = (uint16_t)(2000 + bcd2dec(buf[6]));
}

uint8_t rtc_second(void) { return _sec; }
uint8_t rtc_minute(void) { return _min; }
uint8_t rtc_hour(void)   { return _hour; }
uint8_t rtc_day(void)    { return _day; }
uint8_t rtc_month(void)  { return _month; }
uint16_t rtc_year(void)  { return _year; }
uint8_t rtc_dow(void)    { return _dow; }
