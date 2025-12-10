#include <stm32u0xx_hal.h>

#include "rtos/Publisher.hpp"
#include "rtos/Thread.hpp"
#include "topic/Topics.hpp"

using namespace rtos;

class Button : Thread<1024> {
    Publisher<topic::message::Led> publisher;
    bool last;

    void thread() {
        topic::message ::Led message;

        while(true) {
            const bool current = (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET);

            if(current != last) {
                message.state = current;
                publisher.publish(message);
            }

            last = current;

            delay(50);
        }
    }

public:
    Button() : Thread{"button", Thread::Priority::Idle}, publisher{topic::LedControl}, last{false} {
    }
};

static Button button;
