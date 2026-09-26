#include "stepper.h"
#include <math.h>

/* 8-state half-step sequence, same table AccelStepper's HALFSTEP mode uses. */
static const uint8_t HALF_STEP_TABLE[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1},
};

static void apply_step(stepper_t *s, int8_t index)
{
    for (uint8_t i = 0; i < 4; i++)
        gpio_write(s->pins[i], HALF_STEP_TABLE[index][i]);
}

void stepper_init(stepper_t *s, gpio_pin_t p1, gpio_pin_t p2, gpio_pin_t p3, gpio_pin_t p4)
{
    s->pins[0] = p1;
    s->pins[1] = p2;
    s->pins[2] = p3;
    s->pins[3] = p4;

    for (uint8_t i = 0; i < 4; i++) gpio_set_output(s->pins[i]);

    s->step_index = 0;
    s->direction = 0;
    s->step_interval_ms = 0;
    s->last_step_ms = 0;

    apply_step(s, 0);
}

void stepper_set_speed(stepper_t *s, float steps_per_sec)
{
    if (steps_per_sec == 0.0f)
    {
        s->direction = 0;
        return;
    }

    s->direction = (steps_per_sec > 0) ? 1 : -1;

    float abs_speed = fabsf(steps_per_sec);
    s->step_interval_ms = (unsigned long)(1000.0f / abs_speed);
    if (s->step_interval_ms == 0) s->step_interval_ms = 1;
}

void stepper_run(stepper_t *s, unsigned long now_ms)
{
    if (s->direction == 0) return;

    if (now_ms - s->last_step_ms >= s->step_interval_ms)
    {
        s->last_step_ms = now_ms;
        s->step_index = (int8_t)(((s->step_index + s->direction) + 8) % 8);
        apply_step(s, s->step_index);
    }
}
