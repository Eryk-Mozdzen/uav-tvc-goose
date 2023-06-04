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
	events::StateLimits limits;
	events::Movement movement;
	events::Negation correct;
	events::Negation connect;
	events::Negation stil;
	events::Combination<3> readiness;
	events::AltitudeReached alt_reached;
	events::Negation alt_timeout;

	states::Abort abort;
	states::Ready ready;
	states::Active active;
	states::TakeOff takeoff;
	states::Landing landing;

	sm::StateMachine<5, 4> sm;

	IntervalLogger<comm::Controller> telemetry_controller;

public:
	Control();

	void task();
};

Control control;

Control::Control() : TaskClassS{"control loop", TaskPrio_Mid},
		cmd_start{comm::Command::START},
		cmd_land{comm::Command::LAND},
		disconnect{1000},
		limits{30.f, 2.f},
		movement{10.f},
		correct{&limits, 3000},
		connect{&disconnect, 3000},
		stil{&movement, 3000},
		readiness{&correct, &connect, &stil},
		alt_reached{1.f, 0.2f, 1000},
		alt_timeout(&alt_reached, 3000),
		sm{&abort},
		telemetry_controller{"controller telememetry", Transfer::ID::TELEMETRY_CONTROLLER} {

	sm.transit(&abort, &ready, &readiness);
	sm.transit(&ready, &abort, &limits);
	sm.transit(&ready, &abort, &disconnect);
	sm.transit(&ready, &abort, &movement);
	sm.transit(&ready, &takeoff, &cmd_start);

	sm.transit(&takeoff, &abort, &limits);
	sm.transit(&takeoff, &active, &alt_reached);
	sm.transit(&takeoff, &landing, &alt_timeout);
	sm.transit(&takeoff, &landing, &disconnect);

	sm.transit(&active, &abort, &limits);
	sm.transit(&active, &landing, &cmd_land);
	sm.transit(&active, &landing, &disconnect);

	sm.transit(&landing, &abort, &limits);
	sm.transit(&landing, &ready, &stil);
}

void Control::task() {

	sm.start();

	comm::Controller::State setpoint;
	comm::Controller::State process_value;

	TickType_t time = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&time, 10);

		Transfer::FrameRX frame;
		while(Transport::getInstance().frame_rx_queue.pop(frame, 0)) {
			if(frame.id==Transfer::ID::CONTROL_COMMAND) {
				cmd_start.feed(frame);
				cmd_land.feed(frame);
			}

			if(frame.id==Transfer::ID::CONTROL_SETPOINT) {
				frame.getPayload(setpoint);
				disconnect.reset();
			}
		}

		while(Transport::getInstance().state_queue.pop(process_value, 0));

		comm::Controller controller_data;
		controller_data.setpoint = setpoint;
		controller_data.process_value = process_value;
		controller_data.angles[0] = 0.16f*sinf(2.f*3.1415f*time*0.001f*0.3f);
		controller_data.angles[1] = 0.16f*sinf(2.f*3.1415f*time*0.001f*0.3f + 0.5f*3.1415f);
		controller_data.angles[2] = 0.16f*sinf(2.f*3.1415f*time*0.001f*0.3f + 3.1415f);
		controller_data.angles[3] = 0.16f*sinf(2.f*3.1415f*time*0.001f*0.3f + 1.5f*3.1415f);
		controller_data.throttle = 0.5f*(sinf(2.f*3.1415f*time*0.001f*0.1f) + 1.f);
		controller_data.state = states::getCurrent();

		telemetry_controller.feed(controller_data);

		limits.check(process_value);
		movement.check(process_value);
		correct.check();
		connect.check();
		stil.check();
		readiness.check();
		alt_reached.check(process_value);
		alt_timeout.check();
		sm.update();
	}
}
