#include "stm32f4xx_hal.h"
#include "TaskCPP.h"
#include "transport.h"
#include "transfer.h"
#include "communication_bus.h"

class WirelessTransmitter : public TaskClassS<1024> {
public:

	WirelessTransmitter();

	void task();
};

TaskHandle_t wireless_transmitter_task;

WirelessTransmitter wireless_transmitter;

WirelessTransmitter::WirelessTransmitter() : TaskClassS{"wireless TX", TaskPrio_High} {

}

void WirelessTransmitter::task() {
	wireless_transmitter_task = getTaskHandle();

	while(1) {
		Transfer::FrameTX tx;
		Transport::getInstance().wireless_tx_queue.pop(tx, portMAX_DELAY);

		CommunicationBus::getInstance().write(tx.buffer, tx.length);
	}
}
