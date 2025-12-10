#ifndef RTOS_THREAD_HPP
#define RTOS_THREAD_HPP

#include <FreeRTOS.h>
#include <task.h>

namespace rtos {

template <uint32_t STACK>
class Thread {
    StackType_t stack[STACK / sizeof(StackType_t)];
    StaticTask_t tcb;
    TaskHandle_t handle;

    virtual void thread() = 0;

    static void function(void *param) {
        Thread<STACK> *thread = static_cast<Thread<STACK> *>(param);

        thread->thread();
    }

protected:
    void delay(const TickType_t ticks) {
        vTaskDelay(ticks);
    }

public:
    enum Priority {
        Idle = 0,
        Low = 1,
        Mid = 3,
        High = 4,
    };

    Thread(const char *name, const Priority priority) {
        handle = xTaskCreateStatic(function, name, STACK / sizeof(StackType_t), this, priority,
                                   stack, &tcb);
    }
};

}

#endif
