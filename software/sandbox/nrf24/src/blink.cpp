#include <stm32u0xx_hal.h>

#include "rtos/Thread.hpp"

using namespace rtos;

class Blink : Thread<1024> {
public:
    Blink() : Thread{"blink", rtos::ThreadPriority::Idle} {
    }

    void thread() {
        while(true) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
            delay(50);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
            delay(950);
        }
    }
};

static Blink blink;
