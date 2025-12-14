#include <cstring>

#include <FreeRTOS.h>
#include <message_buffer.h>
#include <semphr.h>

#include "rtos/Log.hpp"
#include "rtos/Publisher.hpp"
#include "rtos/Stream.hpp"
#include "rtos/Thread.hpp"
#include "rtos/Topics.hpp"

namespace rtos {

Log log;

Log::Log()
    : Thread{"rtos logger", Thread::High}, Stream{line, sizeof(line)}, publisher{topics::Logs} {
    bufferHandle = xMessageBufferCreateStatic(sizeof(buffer), buffer, &bufferStorage);
    mutexHandle = xSemaphoreCreateMutexStatic(&mutexStorage);
}

void Log::thread() {
    rtos::log << rtos::acquire << "----- SYSTEM RESET ----- " << rtos::endl << rtos::release;

    while(true) {
        message.len =
            xMessageBufferReceive(bufferHandle, message.str, messages::Log::LENGTH, portMAX_DELAY);

        publisher.publish(message);

        delay(10);
    }
}

Log &Log::operator<<(Log &(*manip)(Log &)) {
    return manip(*this);
}

Log &acquire(Log &log) {
    xSemaphoreTake(log.mutexHandle, portMAX_DELAY);
    return log;
}

Log &release(Log &log) {
    xSemaphoreGive(log.mutexHandle);
    return log;
}

Log &endl(Log &log) {
    // memset(log.line, ' ', 8);
    // writeInt(log.line, log.counter);
    xMessageBufferSend(log.bufferHandle, log.line, log.length(), portMAX_DELAY);
    log.clear();
    // log.counter++;
    return log;
}

Log &operator<<(Log &log, const HAL_StatusTypeDef variable) {
    switch(variable) {
        case HAL_OK: {
            log << "HAL_OK";
        } break;
        case HAL_BUSY: {
            log << "HAL_BUSY";
        } break;
        case HAL_TIMEOUT: {
            log << "HAL_TIMEOUT";
        } break;
        case HAL_ERROR: {
            log << "HAL_ERROR";
        } break;
    }

    return log;
}

}
