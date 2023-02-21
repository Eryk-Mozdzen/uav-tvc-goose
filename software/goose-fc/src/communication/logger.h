#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	LOG_DEBUG = 7,
	LOG_INFO = 14,
	LOG_WARNING = 11,
	LOG_ERROR = 9
} log_type_t;

void LOG(const log_type_t type, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
