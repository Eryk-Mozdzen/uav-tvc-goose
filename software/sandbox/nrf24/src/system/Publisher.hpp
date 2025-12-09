#ifndef SYSTEM_PUBLISHER_HPP
#define SYSTEM_PUBLISHER_HPP

#include "system/Topic.hpp"

namespace system {

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
