#include "stm32f4xx_hal.h"
#include "TaskCPP.h"

extern "C" {
	#include "usb_device.h"
	#include "usbd_cdc_if.h"
}

#include "transport.h"
#include "transfer.h"

class WireTransmitter : public TaskClassS<1024> {
public:

	WireTransmitter();

	void task();
};

TaskHandle_t wire_transmitter_task;

WireTransmitter wire_transmitter;

WireTransmitter::WireTransmitter() : TaskClassS{"wire TX", TaskPrio_High} {

}

void WireTransmitter::task() {
	wire_transmitter_task = getTaskHandle();

	USB_DEVICE_Init();

	while(1) {
		Transfer::FrameTX tx;
		Transport::getInstance().wire_tx_queue.pop(tx, portMAX_DELAY);

		do {
			CDC_Transmit_FS(tx.buffer, tx.length);
		} while(!ulTaskNotifyTakeIndexed(0, pdTRUE, 500));
	}
}
