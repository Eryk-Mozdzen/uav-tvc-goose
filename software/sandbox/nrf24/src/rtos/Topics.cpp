#include "rtos/Topic.hpp"
#include "rtos/messages/Led.hpp"
#include "rtos/messages/Log.hpp"

namespace rtos::topics {

Topic<messages::Led> LedControl;
Topic<messages::Log> Logs;

}
