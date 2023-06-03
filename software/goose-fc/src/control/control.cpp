#include "TaskCPP.h"

#include "transport.h"
#include "transfer.h"

#include "state_machine.h"
#include "events.h"
#include "states.h"
#include "interval_logger.h"

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

	IntervalLogger<Transfer::Controller> telemetry_controller;

public:
	Control();

	void task();
};

Control control;

Control::Control() : TaskClassS{"control loop", TaskPrio_Mid},
		cmd_start{Transfer::Command::START},
		cmd_land{Transfer::Command::LAND},
		disconnect{2000},
		limits{30.f, 2.f},
		sm{&abort},
		telemetry_controller{"controller telememetry", Transfer::ID::CONTROLLER} {

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

		Transfer::Controller controller;
		controller.angles[0] = 0.16f*sinf(2.f*3.1415f*time*0.001f*0.3f);
		controller.angles[1] = 0.16f*sinf(2.f*3.1415f*time*0.001f*0.3f + 0.5f*3.1415f);
		controller.angles[2] = 0.16f*sinf(2.f*3.1415f*time*0.001f*0.3f + 3.1415f);
		controller.angles[3] = 0.16f*sinf(2.f*3.1415f*time*0.001f*0.3f + 1.5f*3.1415f);
		controller.throttle = 0.5f*(sinf(2.f*3.1415f*time*0.001f*0.1f) + 1.f);
		//controller.sm_state = 

		telemetry_controller.feed(controller);

		sm.update();
	}
}
