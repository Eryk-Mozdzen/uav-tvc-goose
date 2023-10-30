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
        uint16_t min_compare;
        uint16_t max_compare;

        inline uint16_t getCenter() const {
            return (max_compare + min_compare)/2;
        }

        inline uint16_t getRadius() const {
            return (max_compare - min_compare)/2;
        }
    };

    static constexpr Servo servos[4] = {
        {TIM_CHANNEL_1, 1000, 2000},
        {TIM_CHANNEL_2, 1000, 2000},
        {TIM_CHANNEL_3, 1000, 2000},
        {TIM_CHANNEL_4, 1000, 2000}
    };

    struct ESC {
        static constexpr uint16_t min_compare = 1000;
        static constexpr uint16_t max_compare = 2000;
    };

    TIM_HandleTypeDef htim3_servo;
    TIM_HandleTypeDef htim1_esc;

    Ramp throttle_ramp;

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
