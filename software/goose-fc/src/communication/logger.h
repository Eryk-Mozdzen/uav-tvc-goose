#pragma once

class Logger {
	Logger();

public:
	enum LOG {
		DEBUG = 7,
		INFO = 14,
		WARNING = 11,
		ERROR = 9
	};

	void log(const LOG type, const char *format, ...);

	static Logger & getInstance();
};
