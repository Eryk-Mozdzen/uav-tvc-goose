#include <cstring>

#include <FreeRTOS.h>
#include <message_buffer.h>
#include <semphr.h>

#include "rtos/Log.hpp"
#include "rtos/Publisher.hpp"
#include "rtos/Thread.hpp"
#include "rtos/Topics.hpp"

static int writeInt(char *str, const int variable) {
    if(variable == 0) {
        *str = '0';
        return 1;
    }

    char buffer[16];
    int val = variable;
    int i = 0;

    while(val > 0) {
        buffer[i++] = '0' + (val % 10);
        val /= 10;
    }

    const int len = i;

    while(i--) {
        *str = buffer[i];
        str++;
    }

    return len;
}

namespace rtos {

Log log;

Log::Log() : Thread{"rtos logger", Thread::High}, index{8}, counter{0}, publisher{topics::Logs} {
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

Log &operator<<(Log &log, const LogCommand &command) {
    switch(command) {
        case LogCommand::acquire: {
            xSemaphoreTake(log.mutexHandle, portMAX_DELAY);
        } break;
        case LogCommand::release: {
            xSemaphoreGive(log.mutexHandle);
        } break;
        case LogCommand::endl: {
            memset(log.line, ' ', 8);
            writeInt(log.line, log.counter);
            xMessageBufferSend(log.bufferHandle, log.line, log.index, portMAX_DELAY);
            log.index = 8;
            log.counter++;
        } break;
    }

    return log;
}

Log &operator<<(Log &log, const char variable) {
    const uint32_t available = sizeof(log.line) - log.index;

    if(available > 0) {
        log.line[log.index] = variable;
        log.index++;
    }

    return log;
}

Log &operator<<(Log &log, const char *variable) {
    const uint32_t len = strlen(variable);
    const uint32_t available = sizeof(log.line) - log.index;
    const uint32_t write = (available > len) ? len : available;

    memcpy(&log.line[log.index], variable, write);

    log.index += write;

    return log;
}

Log &operator<<(Log &log, const bool variable) {
    log << (variable ? "true" : "false");
    return log;
}

Log &operator<<(Log &log, const int variable) {
    if(variable > 0) {
        log << '+';
        log.index += writeInt(&log.line[log.index], variable);
    } else if(variable < 0) {
        log << '-';
        log.index += writeInt(&log.line[log.index], -variable);
    } else {
        log << "+0";
        return log;
    }

    return log;
}

Log &operator<<(Log &log, const float variable) {
    float value = variable;

    if(value < 0.f) {
        log << '-';
        value = -value;
    } else {
        log << '+';
    }

    const int integer = (int)value;
    const float frac = value - (float)integer;
    const int decimals = (int)(frac * 1000.f + 0.5f);

    log.index += writeInt(&log.line[log.index], integer);

    log << '.';

    if(decimals < 100) {
        log << '0';
    }

    if(decimals < 10) {
        log << '0';
    }

    log.index += writeInt(&log.line[log.index], decimals);

    return log;
}

Log &operator<<(Log &log, const uint8_t variable) {
    log.index += writeInt(&log.line[log.index], variable);
    return log;
}

Log &operator<<(Log &log, const uint32_t variable) {
    log.index += writeInt(&log.line[log.index], variable);
    return log;
}

Log &operator<<(Log &log, const int32_t variable) {
    if(variable > 0) {
        log << '+';
        log.index += writeInt(&log.line[log.index], variable);
    } else if(variable < 0) {
        log << '-';
        log.index += writeInt(&log.line[log.index], -variable);
    } else {
        log << "+0";
        return log;
    }

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
