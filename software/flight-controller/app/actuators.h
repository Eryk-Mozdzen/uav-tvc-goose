#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <stdint.h>

typedef struct {
    float throttle_target;
    float throttle_current;
    float angles[3];
    uint32_t last_time;
} actuators_ctx_t;

void actuators_set(actuators_ctx_t *actuators, float throttle, float *angles);
void actuators_tick(actuators_ctx_t *actuators, const uint32_t time);
void actuators_stop(actuators_ctx_t *actuators);
void actuators_set_compare(const uint32_t esc, const uint32_t *servos);

#endif
