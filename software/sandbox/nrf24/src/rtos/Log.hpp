#ifndef RTOS_LOG_HPP
#define RTOS_LOG_HPP

#include <cstdint>

#define RTOS_LOG(...) rtos::log(__FILE__, __LINE__, __VA_ARGS__)

namespace rtos {

void log(const char *filepath, const uint32_t line, const char *format, ...);

}

#endif
