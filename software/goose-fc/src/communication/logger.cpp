#include "logger.h"

#include <cmath>
#include <cstdio>
#include <cstdarg>
#include <cstring>

#include "transport.h"

Logger::Logger() {

}

void Logger::log(const LOG type, const char *format, ...) {
	va_list args;
    va_start(args, format);

	char buffer[128] = {0};

	const int used = snprintf(buffer, sizeof(buffer), "%c[38;5;%dm", 27, type);

	vsnprintf(buffer + used, sizeof(buffer) - used, format, args);

	Transport::getInstance().tx_queue.add(Transport::TX::LOG, Message{buffer}, 0);
}

Logger & Logger::getInstance() {
	static Logger logger;

	return logger;
}
