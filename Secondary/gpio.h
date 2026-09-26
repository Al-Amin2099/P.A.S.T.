#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <avr/io.h>

/* Bundles a pin's DDR/PORT/PIN registers so stepper.c can drive pins
 * on PORTB, PORTC, or PORTD interchangeably without caring which. */
typedef struct {
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *pinreg;
    uint8_t bit;
} gpio_pin_t;

static inline void gpio_set_output(gpio_pin_t p) { *p.ddr |= (uint8_t)(1 << p.bit); }
static inline void gpio_set_input(gpio_pin_t p)  { *p.ddr &= (uint8_t)~(1 << p.bit); }

static inline void gpio_write(gpio_pin_t p, uint8_t v)
{
    if (v) *p.port |= (uint8_t)(1 << p.bit);
    else   *p.port &= (uint8_t)~(1 << p.bit);
}

static inline uint8_t gpio_read(gpio_pin_t p)
{
    return (*p.pinreg & (1 << p.bit)) ? 1 : 0;
}

/* Arduino Nano digital pin -> AVR port/bit, for the pins this project uses.
 * Same mapping as the Uno: D0-D7 = PORTD, D8-D13 = PORTB, A0-A5 = PORTC. */
#define PIN_D2  (gpio_pin_t){&DDRD, &PORTD, &PIND, PD2}
#define PIN_D3  (gpio_pin_t){&DDRD, &PORTD, &PIND, PD3}
#define PIN_D4  (gpio_pin_t){&DDRD, &PORTD, &PIND, PD4}
#define PIN_D5  (gpio_pin_t){&DDRD, &PORTD, &PIND, PD5}
#define PIN_D6  (gpio_pin_t){&DDRD, &PORTD, &PIND, PD6}
#define PIN_D7  (gpio_pin_t){&DDRD, &PORTD, &PIND, PD7}
#define PIN_D8  (gpio_pin_t){&DDRB, &PORTB, &PINB, PB0}
#define PIN_D9  (gpio_pin_t){&DDRB, &PORTB, &PINB, PB1}
#define PIN_D10 (gpio_pin_t){&DDRB, &PORTB, &PINB, PB2}
#define PIN_D11 (gpio_pin_t){&DDRB, &PORTB, &PINB, PB3}
#define PIN_A0  (gpio_pin_t){&DDRC, &PORTC, &PINC, PC0}
#define PIN_A1  (gpio_pin_t){&DDRC, &PORTC, &PINC, PC1}
#define PIN_A2  (gpio_pin_t){&DDRC, &PORTC, &PINC, PC2}
#define PIN_A3  (gpio_pin_t){&DDRC, &PORTC, &PINC, PC3}

#endif
