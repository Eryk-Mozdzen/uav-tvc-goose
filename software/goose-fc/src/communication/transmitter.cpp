#include "stm32f4xx_hal.h"
#include "TaskCPP.h"

extern "C" {
	#include "usb_device.h"
	#include "usbd_cdc_if.h"
}

#include "transport.h"
#include "logger.h"
#include "transfer.h"

class Transmitter : public TaskClassS<1024> {
public:

	Transmitter();

	void task();
};

TaskHandle_t transmitter_task;

Transmitter transmitter;

Transmitter::Transmitter() : TaskClassS{"TX", TaskPrio_Mid} {
	
}

void Transmitter::task() {
	transmitter_task = getTaskHandle();

	USB_DEVICE_Init();

	while(1) {
		Transfer::FrameTX tx;
		Transport::getInstance().tx_queue.pop(tx, portMAX_DELAY);
		
		do {
			CDC_Transmit_FS(tx.buffer, tx.length);
		} while(!ulTaskNotifyTakeIndexed(0, pdTRUE, 500));

		// TODO
		// send over UART
	}
}
