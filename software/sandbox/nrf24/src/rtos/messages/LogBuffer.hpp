#ifndef RTOS_MESSAGES_LOG_BUFFER_HPP
#define RTOS_MESSAGES_LOG_BUFFER_HPP

#include <cstdint>

namespace rtos::messages {

struct LogBuffer {
    static constexpr uint32_t DEPTH = 16;
    static constexpr uint32_t LENGTH = 128;

    char logs[DEPTH][LENGTH];
};

}

#endif
