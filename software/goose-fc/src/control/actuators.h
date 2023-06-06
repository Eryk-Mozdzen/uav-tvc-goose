#pragma once

#include "stm32f4xx_hal.h"

class Actuators {
    static constexpr uint16_t psc = 96;
    static constexpr uint16_t arr = 20000;

    struct Servo {
        static constexpr uint16_t min_compare = 500;
        static constexpr uint16_t max_compare = 2400;
        static constexpr uint16_t center_compare = min_compare + max_compare/2;
        static constexpr uint16_t radius_compare = (max_compare - min_compare)/2;
    };

    struct ESC {
        static constexpr uint16_t min_compare = 1000;
        static constexpr uint16_t max_compare = 2000;
    };

    static constexpr float pi = 3.1415f;
    static constexpr float max_servo = 0.25f*pi;

    TIM_HandleTypeDef htim3_servo;
    TIM_HandleTypeDef htim1_esc;

    Actuators();

public:

    enum Fin {
        FIN1 = TIM_CHANNEL_1,
        FIN2 = TIM_CHANNEL_2,
        FIN3 = TIM_CHANNEL_3,
        FIN4 = TIM_CHANNEL_4
    };

    Actuators(Actuators &) = delete;
	void operator=(const Actuators &) = delete;

    void init();

    void setFinAngle(const Fin fin, float alpha);
    void setMotorThrottle(float thorttle);

    float getFinAngle(const Fin fin) const;
    float getMotorThrottle() const;

    static  Actuators & getInstace();
};
