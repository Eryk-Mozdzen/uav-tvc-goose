#include <cstdio>
#include <cstring>

#include <stm32u0xx_hal.h>

#include "rtos/Subscriber.hpp"
#include "topic/Topics.hpp"

using namespace rtos;

extern UART_HandleTypeDef huart2;

class Serial : Subscriber<topic::message::Led, 1024> {

    void receive(const topic::message::Led &message) {
        char str[64];
        sprintf(str, "led state: %d\n\r", message.state);

        HAL_UART_Transmit(&huart2, (const uint8_t *)str, strlen(str), HAL_MAX_DELAY);
    }

public:
    Serial() : Subscriber{topic::LedControl, "serial driver", Thread::Priority::High} {
    }
};

static Serial serial;
