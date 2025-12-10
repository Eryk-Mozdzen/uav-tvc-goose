#ifndef TOPIC_TOPICS_HPP
#define TOPIC_TOPICS_HPP

#include "rtos/Topic.hpp"
#include "topic/message/Led.hpp"

namespace topic {

extern rtos::Topic<message::Led> LedControl;

}

#endif
