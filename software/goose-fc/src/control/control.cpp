#include "TaskCPP.h"

#include "transport.h"
#include "transfer.h"
#include "logger.h"

class Control : public TaskClassS<2048> {
public:
	Control();

	void task();
};

Control control;

Control::Control() : TaskClassS{"control loop", TaskPrio_High} {

}

void Control::task() {

	Transfer::FrameRX frame;

	while(1) {
		Transport::getInstance().wireless_rx_queue.pop(frame, portMAX_DELAY);

		Logger::getInstance().log(Logger::INFO, "[length = %u\tid = %u\t]", frame.length, frame.id);
	}
}
