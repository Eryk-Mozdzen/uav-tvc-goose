#ifndef RTOS_TASK_HPP
#define RTOS_TASK_HPP

#include <FreeRTOS.h>
#include <task.h>

namespace rtos {

enum ThreadPriority {
    Idle = 0,
    Low = 1,
    Mid = 3,
    High = 4,
};

template <uint32_t STACK>
class Thread {
    StackType_t stack[STACK];
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
    Thread(const char *name, const ThreadPriority priority) {
        handle = xTaskCreateStatic(function, name, STACK, this, priority, stack, &tcb);
    }
};

}

#endif
