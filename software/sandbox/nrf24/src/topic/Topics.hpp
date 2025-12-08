#ifndef TOPIC_TOPICS_HPP
#define TOPIC_TOPICS_HPP

#include "system/Topic.hpp"
#include "topic/message/Led.hpp"

namespace topic {

extern system::Topic<message::Led> LedControl;

}

#endif
