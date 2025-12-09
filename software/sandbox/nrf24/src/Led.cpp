#include <stm32u0xx_hal.h>

#include "system/Subscriber.hpp"
#include "topic/Topics.hpp"

using namespace system;

class Led : Subscriber<topic::message::Led, 1024> {
public:
    Led() : Subscriber{topic::LedControl, "led driver", Thread::Priority::Idle} {
    }

    void receive(const topic::message::Led &message) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, message.state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
};

static Led led;
