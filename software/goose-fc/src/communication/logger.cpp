#include "logger.h"

#include <cmath>
#include <cstdio>
#include <cstdarg>
#include <cstring>

Logger::Logger() {

}

void Logger::log(const LOG type, const char *format, ...) {
	va_list args;
    va_start(args, format);

	char buffer[128] = {0};
	const size_t len = vsnprintf(buffer, sizeof(buffer), format, args);

	const Transfer::FrameTX frame = Transfer::encode(buffer, len, static_cast<Transfer::ID>(type));

	Transport::getInstance().wire_tx_queue.add(frame, 0);
	Transport::getInstance().wireless_tx_queue.add(frame, 0);
}

Logger & Logger::getInstance() {
	static Logger logger;

	return logger;
}
