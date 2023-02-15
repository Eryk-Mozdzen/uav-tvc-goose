#include "communication.h"

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "usb_device.h"
#include "usbd_cdc_if.h"

#include "logger.h"
#include "queue_element.h"
#include <string.h>

TaskHandle_t transmitterTask;
QueueHandle_t tx_msg_queue;
QueueHandle_t rx_msg_queue;

void transmitter(void *param) {
	(void)param;

	transmitterTask = xTaskGetCurrentTaskHandle();

	queue_element_t tx_msg;

	while(1) {
		xQueueReceive(tx_msg_queue, &tx_msg, portMAX_DELAY);

		switch(tx_msg.type) {
			case TX_LOG: {

				CDC_Transmit_FS(tx_msg.data, strlen(tx_msg.data));
				ulTaskNotifyTakeIndexed(0, pdTRUE, 1000);

			} break;
			case TX_TELEMETRY: {

				// TODO

			} break;
			default: {
				LOG(LOG_ERROR, "TX unable to resolve type: %d\n\r", tx_msg.type);
			} break;
		}

		vPortFree(tx_msg.data);
	}
}

/*void receiver(void *param) {
	(void)param;

	controls_t controls;

	while(1) {

	}
}*/

void Communication_Init() {

	tx_msg_queue = xQueueCreate(16, sizeof(queue_element_t));
	rx_msg_queue = xQueueCreate(16, sizeof(controls_t));

	USB_DEVICE_Init();

	xTaskCreate(transmitter, "TX", 512, NULL, 4, NULL);
	//xTaskCreate(receiver, "RX", 512, NULL, 4, NULL);
}
