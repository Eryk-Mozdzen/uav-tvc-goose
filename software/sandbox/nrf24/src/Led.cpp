#include <stm32u0xx_hal.h>

#include "rtos/Subscriber.hpp"
#include "topic/Topics.hpp"

using namespace rtos;

class Led : Subscriber<topic::message::Led, 1024> {

    void receive(const topic::message::Led &message) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, message.state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }

public:
    Led() : Subscriber{topic::LedControl, "led driver", Thread::Priority::Idle} {
    }
};

static Led led;
