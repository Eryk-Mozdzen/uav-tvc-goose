#ifndef RTOS_TOPICS_HPP
#define RTOS_TOPICS_HPP

#include "rtos/Topic.hpp"
#include "rtos/messages/Led.hpp"
#include "rtos/messages/Log.hpp"

namespace rtos::topics {

extern Topic<messages::Led> LedControl;
extern Topic<messages::Log> Logs;

}

#endif
