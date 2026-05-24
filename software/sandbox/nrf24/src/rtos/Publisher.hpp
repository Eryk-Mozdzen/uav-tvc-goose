#ifndef RTOS_PUBLISHER_HPP
#define RTOS_PUBLISHER_HPP

#include "rtos/Topic.hpp"

namespace rtos {

template <typename MESSAGE>
class Publisher {
    Topic<MESSAGE> &topic;

public:
    Publisher(Topic<MESSAGE> &topic) : topic{topic} {
    }

    void publish(const MESSAGE &message) {
        topic.publish(message);
    }
};

}

#endif
