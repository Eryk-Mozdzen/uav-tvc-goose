#include "stm32f4xx_hal.h"
#include "TaskCPP.h"
#include "transport.h"
#include "transfer.h"

class WirelessReceiver : public TaskClassS<1024> {
public:

	WirelessReceiver();

	void task();
};

WirelessReceiver transmitter;

WirelessReceiver::WirelessReceiver() : TaskClassS{"RX", TaskPrio_Mid} {

}

void WirelessReceiver::task() {

}
