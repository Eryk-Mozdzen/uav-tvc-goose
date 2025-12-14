#ifndef RTOS_LOG_HPP
#define RTOS_LOG_HPP

#include <stm32u0xx_hal.h>

#include <FreeRTOS.h>
#include <message_buffer.h>
#include <semphr.h>

#include "rtos/Publisher.hpp"
#include "rtos/Thread.hpp"
#include "rtos/Topics.hpp"

#define RTOS_ASSERT(expr, expected)                                                                \
    {                                                                                              \
        const auto value = (expr);                                                                 \
        if(value != (expected)) {                                                                  \
            rtos::log << rtos::acquire << __FILE_NAME__ << ":" << __LINE__                         \
                      << " " #expr " failed: " << value << rtos::endl                              \
                      << rtos::release;                                                            \
        }                                                                                          \
    }

namespace rtos {

enum LogCommand {
    acquire,
    release,
    endl,
};

class Log : Thread<512> {
    char line[messages::Log::LENGTH];
    uint32_t index;
    uint32_t counter;

    uint8_t buffer[1024];
    StaticMessageBuffer_t bufferStorage;
    MessageBufferHandle_t bufferHandle;

    Publisher<messages::Log> publisher;
    messages::Log message;

    StaticSemaphore_t mutexStorage;
    SemaphoreHandle_t mutexHandle;

    void thread();

public:
    Log();

    friend Log &operator<<(Log &log, const LogCommand &command);
    friend Log &operator<<(Log &log, const char variable);
    friend Log &operator<<(Log &log, const char *variable);
    friend Log &operator<<(Log &log, const bool variable);
    friend Log &operator<<(Log &log, const int variable);
    friend Log &operator<<(Log &log, const float variable);
    friend Log &operator<<(Log &log, const uint8_t variable);
    friend Log &operator<<(Log &log, const uint32_t variable);
    friend Log &operator<<(Log &log, const int32_t variable);
    friend Log &operator<<(Log &log, const HAL_StatusTypeDef variable);
};

Log &operator<<(Log &log, const LogCommand &command);
Log &operator<<(Log &log, const char variable);
Log &operator<<(Log &log, const char *variable);
Log &operator<<(Log &log, const bool variable);
Log &operator<<(Log &log, const int variable);
Log &operator<<(Log &log, const float variable);
Log &operator<<(Log &log, const uint8_t variable);
Log &operator<<(Log &log, const uint32_t variable);
Log &operator<<(Log &log, const int32_t variable);
Log &operator<<(Log &log, const HAL_StatusTypeDef variable);

extern Log log;

}

#endif
