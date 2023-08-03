#pragma once

#include "stm32f4xx_hal.h"

namespace Lights {

    enum Light {
        Red = GPIO_PIN_14,
		Green = GPIO_PIN_13,
		Blue = GPIO_PIN_12
    };

    void init();

    void set(const Light light, const bool state);
	void toggle(const Light light);
}
