#include "stm32f4xx_hal.h"
#include "TaskCPP.h"
#include "transport.h"
#include "transfer.h"
#include "communication_bus.h"

class WirelessReceiver : public TaskClassS<2048> {
public:

	WirelessReceiver();

	void task();
};

WirelessReceiver wireless_receiver;

WirelessReceiver::WirelessReceiver() : TaskClassS{"wireless RX", TaskPrio_High} {

}

void WirelessReceiver::task() {

	Transfer transfer;

	uint8_t buffer[8];
	Transfer::FrameRX frame;

	while(1) {
		CommunicationBus::getInstance().read(buffer, sizeof(buffer));

		for(uint32_t i=0; i<sizeof(buffer); i++) {
			transfer.consume(buffer[i]);

			if(transfer.receive(frame)) {
				Transport::getInstance().frame_rx_queue.push(frame, 0);
			}
		}
	}
}
