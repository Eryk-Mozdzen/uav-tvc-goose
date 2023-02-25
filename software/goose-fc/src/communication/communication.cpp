#include "communication.h"

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

Communication::Communication() {
	tx_queue = xQueueCreate(16, sizeof(queue_element_t));
	rx_queue = xQueueCreate(16, sizeof(queue_element_t));

	USB_DEVICE_Init();
}

void Communication::run() {
	queue_element_t tx_msg;

	while(1) {
		xQueueReceive(Communication::tx_queue, &tx_msg, portMAX_DELAY);

		switch(tx_msg.type) {
			case Communication::TX::LOG: {

				do {
					CDC_Transmit_FS(tx_msg.data, strlen((const char *)(tx_msg.data)));
				} while(!ulTaskNotifyTakeIndexed(0, pdTRUE, 500));

			} break;
			case Communication::TX::TELEMETRY: {

				// TODO

			} break;
			default: {
				Communication::log(LOG::WARNING, "TX unable to resolve type: %d\n\r", tx_msg.type);
			} break;
		}
	}
}

void Communication::log(const LOG type, const char *format, ...) {
	va_list args;
    va_start(args, format);

	char buffer[128] = {0};

	const int used = snprintf(buffer, sizeof(buffer), "%c[38;5;%dm", 27, type);

	vsnprintf(buffer + used, sizeof(buffer) - used, format, args);

	queue_element_t tx_msg;
	tx_msg.type = TX::LOG;

	const int size = std::max(sizeof(tx_msg.data), strlen(buffer)+1);

	memcpy(tx_msg.data, buffer, size);

	xQueueSend(tx_queue, &tx_msg, 0);
}

Communication& Communication::getInstance() {
	static Communication instance;

	return instance;
}

void transmitterTaskFcn(void *param) {
	(void)param;

	Communication::getInstance().run();
}
