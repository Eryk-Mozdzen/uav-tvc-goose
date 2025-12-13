#include "rtos/Topic.hpp"
#include "rtos/messages/Led.hpp"
#include "rtos/messages/LogBuffer.hpp"

namespace rtos::topics {

Topic<messages::Led> LedControl;
Topic<messages::LogBuffer> Logs;

}
