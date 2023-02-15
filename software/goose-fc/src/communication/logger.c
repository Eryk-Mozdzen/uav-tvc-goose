#include "logger.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "communication.h"
#include "queue_element.h"
#include <string.h>

void LOG(const log_type_t type, const char *format, ...) {
	va_list args;
    va_start(args, format);

	char buffer[128] = {0};

	const int used = snprintf(buffer, sizeof(buffer), "%c[38;5;%dm", 27, type);

	vsnprintf(buffer + used, sizeof(buffer) - used, format, args);

	queue_element_t tx_msg;
	tx_msg.type = TX_LOG;
	tx_msg.data = pvPortMalloc(strlen(buffer)+1);

	memcpy(tx_msg.data, buffer, strlen(buffer)+1);

	xQueueSend(tx_msg_queue, &tx_msg, 0);
}
