/*
    Plain-C port of Star Track's puppet module (originally an Arduino Nano
    sketch using AccelStepper). Reads 6 digital inputs from the master
    board and drives two 28BYJ-48 steppers via ULN2003 driver boards.

    No Arduino core, no AccelStepper - direct register access + a
    hand-rolled half-step driver (stepper.c).
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "gpio.h"
#include "stepper.h"

/* Control inputs from the master board.
 * NOTE: these pin numbers don't need to match the master's pin numbers
 * for the same signal - each board's pins are independent; what matters
 * is that the physical jumper wires connect the right signal to the
 * right pin on each side. This mirrors the original puppet.ino exactly. */
#define STAHP_PIN   PIN_D7
#define CW_PIN      PIN_D6
#define CCW_PIN     PIN_D8
#define STAHP2_PIN  PIN_D10
#define CW2_PIN     PIN_D11
#define CCW2_PIN    PIN_D9

/* Stepper driver pins (ULN2003 IN1-IN4), reordered to (pin1, pin3, pin2, pin4)
 * exactly as the original AccelStepper(HALFSTEP, ...) constructor calls did. */
#define M1_IN1  PIN_D2
#define M1_IN2  PIN_D3
#define M1_IN3  PIN_D4
#define M1_IN4  PIN_D5

#define M2_IN1  PIN_A0
#define M2_IN2  PIN_A1
#define M2_IN3  PIN_A2
#define M2_IN4  PIN_A3

#define STEPPER_SPEED 100.0f /* steps/sec; original sketch used 100 (may
                                 need to drop to 50-75 if yours wobbles
                                 when stopping - see the troubleshooting
                                 notes on the reference project page) */

static volatile unsigned long millis_count = 0;

ISR(TIMER0_COMPA_vect)
{
    millis_count++;
}

static void millis_init(void)
{
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS01) | (1 << CS00); /* prescaler 64 */
    OCR0A = 249;                        /* 1kHz tick at 16MHz */
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

int main(void)
{
    millis_init();

    gpio_set_input(STAHP_PIN);
    gpio_set_input(CW_PIN);
    gpio_set_input(CCW_PIN);
    gpio_set_input(STAHP2_PIN);
    gpio_set_input(CW2_PIN);
    gpio_set_input(CCW2_PIN);

    stepper_t stepper1, stepper2;
    stepper_init(&stepper1, M1_IN1, M1_IN3, M1_IN2, M1_IN4);
    stepper_init(&stepper2, M2_IN1, M2_IN3, M2_IN2, M2_IN4);

    uint8_t stopped = 0;
    uint8_t stopped2 = 0;

    while (1)
    {
        /* motor_roll() equivalent */
        if (gpio_read(STAHP_PIN))
        {
            stopped = 1;
        }
        else
        {
            if (gpio_read(CW_PIN))
            {
                stepper_set_speed(&stepper1, STEPPER_SPEED);
                stopped = 0;
            }
            if (gpio_read(CCW_PIN))
            {
                stepper_set_speed(&stepper1, -STEPPER_SPEED);
                stopped = 0;
            }
        }

        /* motor_pitch() equivalent */
        if (gpio_read(STAHP2_PIN))
        {
            stopped2 = 1;
        }
        else
        {
            if (gpio_read(CW2_PIN))
            {
                stepper_set_speed(&stepper2, STEPPER_SPEED);
                stopped2 = 0;
            }
            if (gpio_read(CCW2_PIN))
            {
                stepper_set_speed(&stepper2, -STEPPER_SPEED);
                stopped2 = 0;
            }
        }

        unsigned long now = millis();

        if (!stopped) stepper_run(&stepper1, now);
        if (!stopped2) stepper_run(&stepper2, now);
    }

    return 0;
}
