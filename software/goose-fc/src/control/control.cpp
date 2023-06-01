#include "TaskCPP.h"

#include "transport.h"
#include "transfer.h"

#include "state_machine.h"
#include "events.h"
#include "states.h"

class Control : public TaskClassS<2048> {
	events::Command cmd_start;
	events::Command cmd_land;
	events::Watchdog disconnect;
	events::StateLimit limits;

	states::Abort abort;
	states::Ready ready;
	states::Active active;
	states::TakeOff takeoff;
	states::Landing landing;

	sm::StateMachine<5, 4> sm;

public:
	Control();

	void task();
};

Control control;

Control::Control() : TaskClassS{"control loop", TaskPrio_Mid},
		cmd_start{Transfer::ID::CONTROL_START},
		cmd_land{Transfer::ID::CONTROL_LAND},
		disconnect{2000},
		limits{30.f, 2.f},
		sm{&abort} {

	sm.transit(&abort, &ready, nullptr);
	sm.transit(&ready, &abort, &limits);
	sm.transit(&ready, &abort, nullptr);
	sm.transit(&ready, &abort, &disconnect);
	sm.transit(&ready, &takeoff, &cmd_start);

	sm.transit(&takeoff, &abort, &limits);
	sm.transit(&takeoff, &active, nullptr);
	sm.transit(&takeoff, &landing, nullptr);
	sm.transit(&takeoff, &landing, &disconnect);

	sm.transit(&active, &abort, &limits);
	sm.transit(&active, &landing, &cmd_land);
	sm.transit(&active, &landing, &disconnect);

	sm.transit(&landing, &abort, &limits);
	sm.transit(&landing, &ready, nullptr);
}

void Control::task() {

	Transfer::FrameRX frame;

	TickType_t time = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&time, 10);

		while(Transport::getInstance().frame_rx_queue.pop(frame, 2)) {
			cmd_start.feed(frame);
			cmd_land.feed(frame);
			disconnect.reset();
		}

		sm.update();
	}
}
