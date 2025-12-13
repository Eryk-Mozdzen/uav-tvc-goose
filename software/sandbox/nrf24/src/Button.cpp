#include <stm32u0xx_hal.h>

#include "rtos/Log.hpp"
#include "rtos/Publisher.hpp"
#include "rtos/Thread.hpp"
#include "rtos/Topics.hpp"

using namespace rtos;

class Button : Thread<1024> {
    Publisher<messages::Led> publisher;
    bool last;

    void thread() {
        messages::Led message;

        while(true) {
            const bool current = (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET);

            if(current != last) {
                RTOS_LOG("guzik %d", current);
                message.state = current;
                publisher.publish(message);
            }

            last = current;

            delay(50);
        }
    }

public:
    Button()
        : Thread{"button", Thread::Priority::Idle}, publisher{topics::LedControl}, last{false} {
    }
};

static Button button;
