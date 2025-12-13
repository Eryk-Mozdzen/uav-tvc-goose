#include <cstdlib>
#include <cstring>

#include <stm32u0xx_hal.h>

#include "rtos/Subscriber.hpp"
#include "rtos/Topics.hpp"

using namespace rtos;

extern UART_HandleTypeDef huart2;

class Serial : Subscriber<messages::LogBuffer, 1024> {
    uint32_t last = 0;
    bool first = true;

    void receive(const messages::LogBuffer &message) {
        const uint8_t crlf[2] = {'\r', '\n'};

        for(uint32_t i = 0; i < messages::LogBuffer::DEPTH; i++) {
            const uint32_t len = strlen(message.logs[i]);
            const uint32_t id = strtol(message.logs[i], NULL, 10);

            if(((id > last) || first) && (len > 0)) {
                HAL_UART_Transmit(&huart2, (const uint8_t *)message.logs[i], len, HAL_MAX_DELAY);
                HAL_UART_Transmit(&huart2, crlf, 2, HAL_MAX_DELAY);

                first = false;
                last = id;
            }
        }
    }

public:
    Serial() : Subscriber{topics::Logs, "serial logger", Thread::Priority::High} {
    }
};

static Serial serial;
