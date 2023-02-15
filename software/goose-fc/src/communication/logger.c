#include "logger.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "usbd_cdc_if.h"

void LOG(const log_type_t type, const char *format, ...) {
	va_list args;
    va_start(args, format);

	char buffer[256];

	const int used = snprintf(buffer, sizeof(buffer), "%c[38;5;%dm", 27, type);

	vsnprintf(buffer + used, sizeof(buffer) - used, format, args);

	CDC_Transmit_FS((uint8_t *)buffer, strlen(buffer));
}
