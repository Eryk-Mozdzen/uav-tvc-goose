#ifndef SYSTEM_TOPIC_HPP
#define SYSTEM_TOPIC_HPP

#include <FreeRTOS.h>
#include <queue.h>

namespace system {

template <typename MESSAGE>
class Topic {
    uint8_t storage[sizeof(MESSAGE)];
    StaticQueue_t queue;
    QueueHandle_t handle;

public:
    Topic() {
        handle = xQueueCreateStatic(1, sizeof(MESSAGE), storage, &queue);
    }

    void put(const MESSAGE &message) {
        xQueueSend(handle, &message, portMAX_DELAY);
    }

    void peek(MESSAGE &message) const {
        xQueuePeek(handle, &message, portMAX_DELAY);
    }
};

}

#endif
