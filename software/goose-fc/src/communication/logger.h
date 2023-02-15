#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
	LOG_DEBUG = 7,
	LOG_INFO = 14,
	LOG_WARNING = 11,
	LOG_ERROR = 9
} log_type_t;

void LOG(const log_type_t type, const char *format, ...);

#endif
