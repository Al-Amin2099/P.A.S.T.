#ifndef STEPPER_H
#define STEPPER_H

#include <stdint.h>
#include "gpio.h"

/*
    Non-blocking half-step driver for a 28BYJ-48 via a ULN2003 board,
    replacing AccelStepper's HALFSTEP mode + setSpeed()/run() pattern.

    Pass pins in the same order the original .ino passed them to
    AccelStepper's constructor (pin1, pin2, pin3, pin4) - the sketch
    reordered the physical driver pins (motorPin1, motorPin3, motorPin2,
    motorPin4) to get the coil sequence right for this specific stepper,
    and that reordering must be preserved here too.
*/
typedef struct {
    gpio_pin_t pins[4];
    int8_t step_index;
    int8_t direction;              /* +1, -1, or 0 = stopped */
    unsigned long step_interval_ms;
    unsigned long last_step_ms;
} stepper_t;

void stepper_init(stepper_t *s, gpio_pin_t p1, gpio_pin_t p2, gpio_pin_t p3, gpio_pin_t p4);

/* steps_per_sec: positive = one direction, negative = the other, 0 = stop.
   Matches the magnitude used in the original sketch (100). */
void stepper_set_speed(stepper_t *s, float steps_per_sec);

/* Call every loop iteration with the current millis() value; only takes
   an actual step once enough time has passed for the configured speed. */
void stepper_run(stepper_t *s, unsigned long now_ms);

#endif
