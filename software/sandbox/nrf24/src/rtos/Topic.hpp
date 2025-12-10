#ifndef RTOS_TOPIC_HPP
#define RTOS_TOPIC_HPP

#include <FreeRTOS.h>
#include <event_groups.h>
#include <semphr.h>

namespace rtos {

template <typename MESSAGE>
class Topic {
    MESSAGE value;
    StaticSemaphore_t mutexStorage;
    StaticEventGroup_t eventStorage;
    SemaphoreHandle_t mutex;
    EventGroupHandle_t event;

public:
    Topic() {
        mutex = xSemaphoreCreateMutexStatic(&mutexStorage);
        event = xEventGroupCreateStatic(&eventStorage);
    }

    void publish(const MESSAGE &message) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        value = message;
        xSemaphoreGive(mutex);
        xEventGroupSetBits(event, 0x01);
    }

    MESSAGE wait() const {
        MESSAGE message;
        xEventGroupWaitBits(event, 0x01, pdTRUE, pdFALSE, portMAX_DELAY);
        xSemaphoreTake(mutex, portMAX_DELAY);
        message = value;
        xSemaphoreGive(mutex);
        return message;
    }
};

}

#endif
