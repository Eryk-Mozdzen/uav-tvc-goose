#include <stm32u0xx_hal.h>

#include "rtos/Log.hpp"
#include "rtos/Subscriber.hpp"
#include "rtos/Topics.hpp"

using namespace rtos;

extern UART_HandleTypeDef huart2;

class Serial : Subscriber<messages::Log, 512> {
    const uint8_t crlf[2] = {'\r', '\n'};

    void receive(const messages::Log &message) {
        RTOS_ASSERT(
            HAL_UART_Transmit(&huart2, (const uint8_t *)message.str, message.len, HAL_MAX_DELAY),
            HAL_OK);
        RTOS_ASSERT(HAL_UART_Transmit(&huart2, crlf, 2, HAL_MAX_DELAY), HAL_OK);
    }

public:
    Serial() : Subscriber{topics::Logs, "serial logger", Thread::Priority::High} {
    }
};

static Serial serial;
