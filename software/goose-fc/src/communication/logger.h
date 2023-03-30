#pragma once

#include "transfer.h"
#include "transport.h"

class Logger {
	Logger();

public:
	enum LOG {
		DEBUG = Transfer::ID::LOG_DEBUG,
		INFO = Transfer::ID::LOG_INFO,
		WARNING = Transfer::ID::LOG_WARNING,
		ERROR = Transfer::ID::LOG_ERROR
	};

	void log(const LOG type, const char *format, ...);

	template<typename T>
	void send(const Transfer::ID id, const T &obj);

	static Logger & getInstance();
};

template<typename T>
void Logger::send(const Transfer::ID id, const T &obj) {
	const Transfer::FrameTX frame = Transfer::encode(obj, id);

	Transport::getInstance().wire_tx_queue.add(frame, 0);
	Transport::getInstance().wireless_tx_queue.add(frame, 0);
}
