#pragma once

#include "transfer.h"

class Logger {
	Logger();

public:
	enum LOG {
		DEBUG = Transfer::LOG_DEBUG,
		INFO = Transfer::LOG_INFO,
		WARNING = Transfer::LOG_WARNING,
		ERROR = Transfer::LOG_ERROR
	};

	void log(const LOG type, const char *format, ...);

	static Logger & getInstance();
};
