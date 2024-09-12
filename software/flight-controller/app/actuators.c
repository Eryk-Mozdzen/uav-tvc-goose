#include "stm32u5xx_hal.h"

#include "actuators.h"
#include "nvm.h"
#include "msg.h"
#include "utils.h"

#define INTERVAL            20
#define ESC_SLOPE           0.333f
#define ESC_INCREMENT       (ESC_SLOPE*INTERVAL*0.001f)
#define SERVO_MAX_ANGLE     (15.f*DEG2RAD)

extern TIM_HandleTypeDef htim1;

static float interpolate(const float in, const float in_lower, const float in_upper, const float out_lower, const float out_upper) {
    return (((out_upper - out_lower)*(in - in_lower))/(in_upper - in_lower)) + out_lower;
}

static uint32_t servo_get_compare(const msg_frame_calibration_t *calibration, const uint8_t index, const float angle) {
	uint32_t compare = 0;

	if(angle>0) {
		compare = interpolate(angle, 0.f, +0.5f*PI, calibration->servos[3*index + 1], calibration->servos[3*index + 0]);
	} else {
		compare = interpolate(angle, -0.5f*PI, 0.f, calibration->servos[3*index + 2], calibration->servos[3*index + 1]);
	}

    return compare;
}

void actuators_init(actuators_ctx_t *actuators) {
    actuators->throttle_target = 0;
    actuators->throttle_current = 0;
    actuators->angles[0] = 0;
    actuators->angles[1] = 0;
    actuators->angles[2] = 0;

    msg_frame_calibration_t calibration;
    nvm_read(0, &calibration, sizeof(calibration));

    const uint32_t servos[3] = {
        calibration.servos[1],
        calibration.servos[4],
        calibration.servos[7],
    };

    actuators_set_compare(1000, servos);
    HAL_Delay(3000);
    actuators_set_compare(calibration.esc[0], servos);
}

void actuators_set(actuators_ctx_t *actuators, float throttle, float *angles) {
    throttle = throttle>1.f ? 1.f : throttle;
    throttle = throttle<0.f ? 0.f : throttle;
    angles[0] = angles[0]>+SERVO_MAX_ANGLE ? +SERVO_MAX_ANGLE : angles[0];
    angles[0] = angles[0]<-SERVO_MAX_ANGLE ? -SERVO_MAX_ANGLE : angles[0];
    angles[1] = angles[1]>+SERVO_MAX_ANGLE ? +SERVO_MAX_ANGLE : angles[1];
    angles[1] = angles[1]<-SERVO_MAX_ANGLE ? -SERVO_MAX_ANGLE : angles[1];
    angles[2] = angles[2]>+SERVO_MAX_ANGLE ? +SERVO_MAX_ANGLE : angles[2];
    angles[2] = angles[2]<-SERVO_MAX_ANGLE ? -SERVO_MAX_ANGLE : angles[2];

    actuators->throttle_target = throttle;
    actuators->angles[0] = angles[0];
    actuators->angles[1] = angles[1];
    actuators->angles[2] = angles[2];

    msg_frame_calibration_t calibration;
    nvm_read(0, &calibration, sizeof(calibration));

    const uint32_t esc = interpolate(actuators->throttle_current, 0, 1, calibration.esc[0], calibration.esc[1]);
    const uint32_t servos[3] = {
        servo_get_compare(&calibration, 0, actuators->angles[0]),
        servo_get_compare(&calibration, 1, actuators->angles[1]),
        servo_get_compare(&calibration, 2, actuators->angles[2]),
    };

    actuators_set_compare(esc, servos);
}

void actuators_stop(actuators_ctx_t *actuators) {
    actuators->throttle_target = 0;
    actuators->throttle_current = 0;
    actuators->angles[0] = 0;
    actuators->angles[1] = 0;
    actuators->angles[2] = 0;

    msg_frame_calibration_t calibration;
    nvm_read(0, &calibration, sizeof(calibration));

    const uint32_t servos[3] = {
        calibration.servos[1],
        calibration.servos[4],
        calibration.servos[7],
    };

    actuators_set_compare(calibration.esc[0], servos);
}

void actuators_tick(actuators_ctx_t *actuators, const uint32_t time) {
    if((time - actuators->last_time)>=INTERVAL) {
        actuators->last_time = time;

        const float diff = actuators->throttle_target - actuators->throttle_current;

        if(diff>ESC_INCREMENT) {
            actuators->throttle_current +=ESC_INCREMENT;
        } else if(diff<-ESC_INCREMENT) {
            actuators->throttle_current -=ESC_INCREMENT;
        } else {
            actuators->throttle_current = actuators->throttle_target;
        }
    }
}

void actuators_set_compare(const uint32_t esc, const uint32_t *servos) {
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, esc);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, servos[0]);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, servos[1]);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, servos[2]);
}
