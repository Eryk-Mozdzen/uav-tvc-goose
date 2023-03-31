#include "stm32f4xx_hal.h"
#include "TaskCPP.h"
#include "transport.h"
#include "transfer.h"
#include "communication_bus.h"

class WirelessReceiver : public TaskClassS<1024> {
public:

	WirelessReceiver();

	void task();
};

WirelessReceiver transmitter;

WirelessReceiver::WirelessReceiver() : TaskClassS{"RX", TaskPrio_Mid} {

}

void WirelessReceiver::task() {

	Transfer transfer;

	uint8_t buffer[32];
	Transfer::FrameRX frame;

	while(1) {
		CommunicationBus::getInstance().read(buffer, sizeof(buffer));

		for(uint32_t i=0; i<sizeof(buffer); i++) {
			transfer.consume(buffer[i]);

			if(transfer.receive(frame)) {
				Transport::getInstance().wireless_rx_queue.push(frame, 0);
			}
		}
	}
}
