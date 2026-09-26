#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// communication protocol header files
#include "uart.h"
#include "i2c.h"
#include "rtc.h"
#include "mpu.h"

/* Pin assignments
 *
 * stahp - D7
 * cw - D8
 * ccw - D6
 * stahp2 - D10
 * cw2 - D11
 * ccw2 - D9
 *
 */

#define STAHP_PIN	PD7
#define CCW_PIN		PD6
#define CW_PIN		PB0
#define CCW2_PIN	PB1
#define STAHP2_PIN	PB2
#define CW2_PIN		PB3

static double M, Y, D, MN, H, S;
static double A, B;
static const double location = -115.287539; // Las Vegas Longitude
static double lstDegrees = 0;
static double lstHours = 0;

static unsigned long timer = 0;
static const float timeStep = 0.01f;

static double pitch = 0;
static double yaw = 0;
static double val = 0;
static double val2 = 0;
static double temp = 0;

static volatile unsigned long millis_count = 0;

ISR(TIMER0_COMPA_vect)
{
	millis_count++;
}

static void millis_init(void)
{
	TCCR0A = (1 << WGM01);
	TCCR0B = (1 << CS01) | (1 << CS00);
	OCR0A = 249;
	TIMSK0 = (1 << OCIE0A);
	sei();
}

static unsigned long millis(void)
{
	unsigned long m;
	cli();
	m = millis_count;
	sei();
	return m;
}

static void delay_ms(unsigned long ms)
{
	while(ms--) _delay_ms(1);
}

static void receiveData(void)
{
	if(uart_available())
	{
		char line[64];
		uart_read_line(line, sizeof(line));

		char *comma = strchr(line, ',');
		if(comma)
		{
			*comma = '\0';
			char *v2str = line;
			char *v1str = comma +  1;

			val = 90 - atof(v1str);
			val2 = atof(v2str);
			temp = val2;
		}

	}

}

static void pitchCheck(void)
{

	if (floor(pitch * 100) / 100 == floor(val * 100) / 100)
        	PORTD |= (1 << STAHP_PIN);
    	else
        	PORTD &= (uint8_t)~(1 << STAHP_PIN);


    	if (floor(pitch * 100) < floor(val * 100))
        	PORTB |= (1 << CW_PIN);
    	else
        	PORTB &= (uint8_t)~(1 << CW_PIN);


    	if (floor(pitch * 100) > floor(val * 100))
        	PORTD |= (1 << CCW_PIN);
    	else
        	PORTD &= (uint8_t)~(1 << CCW_PIN);

}

static void yawCheck(void)
{
    if (floor(yaw * 100) == floor(val2 * 100))
        PORTB |= (1 << STAHP2_PIN);
    else
        PORTB &= (uint8_t)~(1 << STAHP2_PIN);


    if (floor(yaw * 100) < floor(val2 * 100))
        PORTB |= (1 << CW2_PIN);
    else
        PORTB &= (uint8_t)~(1 << CW2_PIN);


    if (floor(yaw * 100) > floor(val2 * 100))
        PORTB |= (1 << CCW2_PIN);
    else
        PORTB &= (uint8_t)~(1 << CCW2_PIN);

}

static void lstTime(void)
{
    M = (double) rtc_month();
    Y = (double) rtc_year();
    D = (double) rtc_day();
    MN = (double) rtc_minute();
    H = (double) rtc_hour();
    S = (double) rtc_second();

    A = (Y - 2000) * 365.242199;
    B = (M - 1) * 30.4368499;

    double JDN2000 = A + B + (D - 1) + (H / 24);
    double decimalTime = H + (MN / 60) + (S / 3600);
    double LST = 100.46 + 0.985647 * JDN2000 + location + 15 * decimalTime;

    lstDegrees = LST - (floor(LST / 360) * 360);
    lstHours = lstDegrees / 15;
}

int main(void)
{
    uart_init(115200);
    i2c_init();
    millis_init();

    /* Set date-time (sec, min, hour, dow, day, month, year) */
    rtc_set(0, 5, 21, 2, 22, 7, 2025);

    if (!mpu_init())
    {
        uart_println("IMU initialization unsuccessful");
        uart_println("Status: ");
        while (1) { }
    }

    DDRD |= (1 << STAHP_PIN) | (1 << CCW_PIN);
    DDRB |= (1 << CW_PIN) | (1 << CCW2_PIN) | (1 << STAHP2_PIN) | (1 << CW2_PIN);

    delay_ms(5000); /* wait before starting */

    mpu_calibrate_gyro(500);

    while (1)
    {
        rtc_refresh();

        if (floor(lstDegrees) == lstDegrees)
        {
            if (lstDegrees > 100)
                val2 = temp + (360 - lstDegrees);
            else
                val2 = temp - lstDegrees;
        }

        lstTime();
        receiveData();
        pitchCheck();
        yawCheck();
        timer = millis();

        if (mpu_update())
        {
            float gyroX = mpu_get_gyro_x();
            float gyroY = mpu_get_gyro_y();

            yaw = yaw + gyroY * timeStep;
            pitch = pitch + gyroX * timeStep;
        }

        uart_print("Yaw = ");
        uart_println_double(yaw, 4);

        uart_print("Pitch = ");
        uart_println_double(pitch, 4);

        uart_print("lstDegrees: ");
        uart_println_double(lstDegrees, 4);

        uart_print("lstHours: ");
        uart_println_double(lstHours, 4);

        unsigned long elapsed = millis() - timer;
        unsigned long target = (unsigned long)(timeStep * 1000);
        if (target > elapsed) delay_ms(target - elapsed);

    }

    return 0;
}
