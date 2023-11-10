#pragma once

#include "ramp.h"
#include "stm32f4xx_hal.h"

class Actuators {
    static constexpr uint16_t psc = 96;
    static constexpr uint16_t arr = 20000;

    static constexpr float pi = 3.1415f;
    static constexpr float deg2rad = pi/180.f;

    struct Servo {
        static constexpr float max = 15.f*deg2rad;

        uint32_t channel;
        float compare_lower;
        float compare_center;
        float compare_upper;
    };

    static constexpr Servo servos[4] = {
        {TIM_CHANNEL_1, 524, 1498, 2501},
        {TIM_CHANNEL_2, 548, 1498, 2421},
        {TIM_CHANNEL_3, 548, 1431, 2446},
        {TIM_CHANNEL_4, 518, 1340, 2245}
    };

    struct ESC {
        static constexpr uint16_t min_compare = 1000;
        static constexpr uint16_t max_compare = 2000;
    };

    TIM_HandleTypeDef htim3_servo;
    TIM_HandleTypeDef htim1_esc;

    Ramp throttle_ramp;

    template<typename T>
    static inline T clamp(const T value, const T lower, const T upper) {
        return value>upper ? upper : value<lower ? lower : value;
    }

    template<typename T>
    static inline T interpolate(const T in, const T in_lower, const T in_upper, const T out_lower, const T out_upper) {
        return (((out_upper - out_lower)*(in - in_lower))/(in_upper - in_lower)) + out_lower;
    }

    Actuators();

public:

    enum Fin {
        FIN1 = 0,
        FIN2 = 1,
        FIN3 = 2,
        FIN4 = 3
    };

    enum Mode {
        PASSTHROUGH,
        RAMP
    };

    Actuators(Actuators &) = delete;
	void operator=(const Actuators &) = delete;

    void init();

    void setFinCompare(const Fin fin, const uint32_t compare);
    void setFinAngle(const Fin fin, float angle);
    void setMotorThrottle(float throttle, const Mode mode);

    float getFinAngle(const Fin fin) const;
    float getMotorThrottle() const;

    static Actuators & getInstance();
};
