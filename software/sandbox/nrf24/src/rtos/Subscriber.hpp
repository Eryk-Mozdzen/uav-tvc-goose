#ifndef RTOS_SUBSCRIBER_HPP
#define RTOS_SUBSCRIBER_HPP

#include "rtos/Thread.hpp"
#include "rtos/Topic.hpp"

namespace rtos {

template <typename MESSAGE, uint32_t STACK>
class Subscriber : public Thread<STACK> {
    const Topic<MESSAGE> &topic;
    MESSAGE message;

    virtual void receive(const MESSAGE &message) = 0;

    void thread() {
        while(true) {
            topic.wait(message);
            receive(message);
        }
    }

public:
    Subscriber(const Topic<MESSAGE> &topic,
               const char *name,
               const typename Thread<STACK>::Priority priority)
        : Thread<STACK>::Thread{name, priority}, topic{topic} {
    }
};

}

#endif
