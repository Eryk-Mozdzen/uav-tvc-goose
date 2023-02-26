#include "transmitter.h"

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

extern "C" {
	#include "usb_device.h"
	#include "usbd_cdc_if.h"
}

#include "queue_element.h"
#include <cmath>
#include <cstdio>
#include <cstdarg>
#include <cstring>

extern Queue<queue_element_t, 16> tx_queue;

TaskHandle_t transmitter_task;

Transmitter transmitter;

Transmitter::Transmitter() : TaskClassS{"TX", TaskPrio_Mid} {
	
}

void Transmitter::init() {
	USB_DEVICE_Init();
}

void Transmitter::task() {
	transmitter_task = getTaskHandle();

	init();

	queue_element_t tx_msg;

	while(1) {
		tx_queue.pop(tx_msg, portMAX_DELAY);

		switch(tx_msg.type) {
			case TX::LOG: {

				do {
					CDC_Transmit_FS(tx_msg.data, strlen((const char *)(tx_msg.data)));
				} while(!ulTaskNotifyTakeIndexed(0, pdTRUE, 500));

			} break;
			case TX::TELEMETRY: {

				// TODO

			} break;
			default: {
				log(LOG::WARNING, "TX unable to resolve type: %d\n\r", tx_msg.type);
			} break;
		}
	}
}

void Transmitter::log(const LOG type, const char *format, ...) {
	va_list args;
    va_start(args, format);

	char buffer[128] = {0};

	const int used = snprintf(buffer, sizeof(buffer), "%c[38;5;%dm", 27, type);

	vsnprintf(buffer + used, sizeof(buffer) - used, format, args);

	queue_element_t tx_msg;
	tx_msg.type = TX::LOG;

	const int size = std::max(sizeof(tx_msg.data), strlen(buffer)+1);

	memcpy(tx_msg.data, buffer, size);

	tx_queue.add(tx_msg, 0);
}
