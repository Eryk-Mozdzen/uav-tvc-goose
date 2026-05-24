#include <stm32u0xx_hal.h>

#include "rtos/Subscriber.hpp"
#include "rtos/Topics.hpp"

using namespace rtos;

class Led : Subscriber<messages::Led, 512> {

    void receive(const messages::Led &message) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, message.state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }

public:
    Led() : Subscriber{topics::LedControl, "led driver", Thread::Priority::Idle} {
    }
};

static Led led;
