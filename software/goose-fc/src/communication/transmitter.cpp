#include "stm32f4xx_hal.h"
#include "TaskCPP.h"

extern "C" {
	#include "usb_device.h"
	#include "usbd_cdc_if.h"
}

#include "transport.h"
#include "logger.h"

class Transmitter : public TaskClassS<1024> {
public:

	Transmitter();

	void init();

	void task();
};

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

	Transport::TX type = Transport::LOG;

	while(1) {
		Transport::getInstance().tx_queue.pop(type, portMAX_DELAY);

		switch(type) {
			case Transport::TX::LOG: {

				Message str;
				Transport::getInstance().tx_queue.getValue(str);

				do {
					CDC_Transmit_FS((uint8_t *)str.c_str(), str.size());
				} while(!ulTaskNotifyTakeIndexed(0, pdTRUE, 500));

			} break;
			case Transport::TX::TELEMETRY: {

				// TODO

			} break;
		}
	}
}
