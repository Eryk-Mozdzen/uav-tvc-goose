#ifndef RTOS_MESSAGES_LOG_HPP
#define RTOS_MESSAGES_LOG_HPP

#include <cstdint>

namespace rtos::messages {

struct Log {
    static constexpr uint32_t LENGTH = 128;

    char str[LENGTH];
    uint32_t len;
};

}

#endif
