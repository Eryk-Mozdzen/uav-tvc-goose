#include "stm32f4xx_hal.h"
#include "usbd_cdc_if.h"
#include "TaskCPP.h"
#include "transport.h"
#include "transfer.h"

class WireReceiver : public TaskClassS<2048> {
public:

	WireReceiver();

	void task();
};

WireReceiver wire_receiver;

WireReceiver::WireReceiver() : TaskClassS{"wire RX", TaskPrio_High} {

}

void WireReceiver::task() {

	Transfer transfer;

	uint8_t byte;
	Transfer::FrameRX frame;

	while(1) {
		Transport::getInstance().wire_rx_queue.pop(byte, portMAX_DELAY);

		transfer.consume(byte);

		if(transfer.receive(frame)) {
			Transport::getInstance().frame_rx_queue.push(frame, 0);
		}

		frame.payload[0] = byte;
		Transport::getInstance().frame_rx_queue.push(frame, 0);
	}
}
