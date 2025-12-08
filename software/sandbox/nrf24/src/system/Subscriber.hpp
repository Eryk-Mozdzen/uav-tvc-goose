#ifndef SYSTEM_SUBSCRIBER_HPP
#define SYSTEM_SUBSCRIBER_HPP

#include "system/Thread.hpp"
#include "system/Topic.hpp"

namespace system {

template <typename MESSAGE, uint32_t STACK>
class Subscriber : public Thread<STACK> {
    const Topic<MESSAGE> &topic;

    virtual void receive(const MESSAGE &message) = 0;

    void thread() {
        MESSAGE message;

        while(true) {
            topic.peek(message);
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
