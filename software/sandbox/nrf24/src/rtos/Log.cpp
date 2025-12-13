#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "rtos/Publisher.hpp"
#include "rtos/Thread.hpp"
#include "rtos/Topics.hpp"

using namespace rtos;

class Counter {
    uint32_t counter;
    StaticSemaphore_t mutexStorage;
    SemaphoreHandle_t mutexHandle;

public:
    Counter() : counter{0} {
        mutexHandle = xSemaphoreCreateMutexStatic(&mutexStorage);
    }

    uint32_t generate() {
        uint32_t value;
        xSemaphoreTake(mutexHandle, portMAX_DELAY);
        value = counter;
        counter++;
        xSemaphoreGive(mutexHandle);
        return value;
    }
};

class Logger : Thread<512> {
    Publisher<messages::LogBuffer> publisher;

    StaticSemaphore_t mutexStorage;
    SemaphoreHandle_t mutexHandle;

    messages::LogBuffer buffer;
    messages::LogBuffer message;
    uint32_t index;

    void thread() {
        while(true) {
            xSemaphoreTake(mutexHandle, portMAX_DELAY);
            memcpy(&message.logs, &buffer.logs[index],
                   (messages::LogBuffer::DEPTH - index) * messages::LogBuffer::LENGTH);
            memcpy(&message.logs[messages::LogBuffer::DEPTH - index - 1], &buffer.logs,
                   index * messages::LogBuffer::LENGTH);
            xSemaphoreGive(mutexHandle);

            publisher.publish(message);

            delay(1000);
        }
    }

public:
    Logger() : Thread<512>("rtos logger", Thread<512>::High), publisher{topics::Logs}, index{0} {
        mutexHandle = xSemaphoreCreateMutexStatic(&mutexStorage);
    }

    void append(const char *str, const uint32_t len) {
        xSemaphoreTake(mutexHandle, portMAX_DELAY);
        memcpy(buffer.logs[index], str, len);
        index++;
        if(index >= messages::LogBuffer::DEPTH) {
            index = 0;
        }
        xSemaphoreGive(mutexHandle);
    }
};

static Counter counter;
static Logger logger;

namespace rtos {

void log(const char *filepath, const uint32_t line, const char *format, ...) {
    const uint32_t timestamp = xTaskGetTickCount();
    const uint32_t id = counter.generate();

    const char *slash = strrchr(filepath, '/');
    const char *file = slash ? slash + 1 : filepath;

    char str[messages::LogBuffer::LENGTH];
    int result;
    uint32_t len = 0;

    result = snprintf(str, sizeof(str), "%8lu %8lu %s:%lu ", id, timestamp, file, line);
    if(result < 0) {
        return;
    }
    len += result;

    va_list args;
    va_start(args, format);

    result = vsnprintf(&str[result], sizeof(str) - len, format, args);

    va_end(args);

    if(result < 0) {
        return;
    }
    len += result;

    logger.append(str, len);
}

}
