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

Control::Control() : TaskClassS{"control loop", TaskPrio_Mid} {

}

void Control::task() {

	Transfer::FrameRX frame;
	int counter = 0;

	while(1) {
		Transport::getInstance().frame_rx_queue.pop(frame, portMAX_DELAY);
		counter++;

		Logger::getInstance().log(Logger::DEBUG, "message num: %d [length = %u\tid = %u\t]", counter, frame.length, frame.id);
	}
}
