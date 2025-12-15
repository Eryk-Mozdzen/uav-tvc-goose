#ifndef RTOS_LOG_HPP
#define RTOS_LOG_HPP

#include <stm32u0xx_hal.h>

#include <FreeRTOS.h>
#include <message_buffer.h>
#include <semphr.h>

#include "rtos/Publisher.hpp"
#include "rtos/Stream.hpp"
#include "rtos/Thread.hpp"
#include "rtos/Topics.hpp"

#define RTOS_ASSERT(expr, expected)                                                                \
    {                                                                                              \
        const auto value = (expr);                                                                 \
        if(value != (expected)) {                                                                  \
            rtos::log << rtos::acquire << __FILE_NAME__ << ":" << rtos::dec << rtos::setw(0)       \
                      << static_cast<uint32_t>(__LINE__) << " " #expr " failed: " << value         \
                      << rtos::endl                                                                \
                      << rtos::release;                                                            \
        }                                                                                          \
    }

namespace rtos {

class Log : Thread<512>, public Stream<Log> {
    char line[128];
    uint8_t buffer[1024];
    StaticMessageBuffer_t bufferStorage;
    MessageBufferHandle_t bufferHandle;

    Publisher<messages::Log> publisher;
    messages::Log message;

    StaticSemaphore_t mutexStorage;
    SemaphoreHandle_t mutexHandle;

    void thread();

public:
    using Stream::operator<<;

    Log();
    Log(const Log &) = delete;
    Log(Log &&) = delete;
    Log &operator=(const Log &) = delete;
    Log &operator=(Log &&) = delete;

    Log &operator<<(Log &(*manip)(Log &));

    friend Log &acquire(Log &log);
    friend Log &release(Log &log);
    friend Log &endl(Log &log);
};

Log &acquire(Log &log);
Log &release(Log &log);
Log &endl(Log &log);

Log &operator<<(Log &log, const HAL_StatusTypeDef variable);

extern Log log;

}

#endif
