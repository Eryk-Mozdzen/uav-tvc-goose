#include "TaskCPP.h"

#include "transport.h"
#include "transfer.h"

#include "state_machine.h"
#include "events.h"
#include "states.h"
#include "interval_logger.h"
#include "actuators.h"
#include "controller.h"
#include "context.h"

class Control : public TaskClassS<2048>, public Context {
	events::Command cmd_start;
	events::Command cmd_land;
	events::Watchdog disconnect;
	events::Limits limits;
	events::Movement movement;
	events::Negation correct;
	events::Negation connect;
	events::Negation stil;
	events::Combination<3> readiness;
	events::Altitude alt_reached;
	events::Negation alt_timeout;

	states::Abort abort;
	states::Ready ready;
	states::Active active;
	states::TakeOff takeoff;
	states::Landing landing;

	sm::StateMachine<5, 4, Context> sm;

	IntervalLogger<comm::Controller> telemetry_controller;

public:
	Control();

	comm::Controller getTelemetry();

	void task();
};

Control control;

Control::Control() : TaskClassS{"control loop", TaskPrio_Mid},
		cmd_start{comm::Command::START},
		cmd_land{comm::Command::LAND},
		disconnect{1000},
		correct{&limits, 3000},
		connect{&disconnect, 3000},
		stil{&movement, 3000},
		readiness{&correct, &connect, &stil},
		alt_timeout(&alt_reached, 6000),
		sm{&abort, this},
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

	Actuators::getInstance().init();

	sm.start();

	TickType_t time = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&time, 10);

		Transfer::FrameRX frame;
		while(Transport::getInstance().frame_rx_queue.pop(frame, 0)) {
			if(frame.id==Transfer::ID::CONTROL_COMMAND) {
				cmd_start.check(frame);
				cmd_land.check(frame);
			}

			if(frame.id==Transfer::ID::CONTROL_SETPOINT) {
				if(frame.getPayload(setpoint)) {
					disconnect.reset();
				}
			}

			if(frame.id==Transfer::ID::CONTROL_MANUAL) {
				comm::Manual manual;
				if(frame.getPayload(manual)) {
					Actuators::getInstance().setMotorThrottle(manual.throttle);
					Actuators::getInstance().setFinAngle(Actuators::FIN1, manual.angles[0]);
					Actuators::getInstance().setFinAngle(Actuators::FIN2, manual.angles[1]);
					Actuators::getInstance().setFinAngle(Actuators::FIN3, manual.angles[2]);
					Actuators::getInstance().setFinAngle(Actuators::FIN4, manual.angles[3]);
				}
			}
		}

		while(Transport::getInstance().state_queue.pop(process_value, 0)) {
			Controller::getInstance().setProcessValue({
				process_value.rpy[0],
				process_value.rpy[1],
				process_value.rpy[2],
				process_value.w[0],
				process_value.w[1],
				process_value.w[2],
				process_value.z,
				process_value.vz
			});
		}

		telemetry_controller.feed(getTelemetry());

		sm.update();
	}
}

comm::Controller Control::getTelemetry() {
	const Matrix<8, 1> controller_sp_raw = Controller::getInstance().getSetpoint();
	comm::Controller::State controller_sp;
	controller_sp.rpy[0] = controller_sp_raw(0, 0);
	controller_sp.rpy[1] = controller_sp_raw(1, 0);
	controller_sp.rpy[2] = controller_sp_raw(2, 0);
	controller_sp.w[0] = controller_sp_raw(3, 0);
	controller_sp.w[1] = controller_sp_raw(4, 0);
	controller_sp.w[2] = controller_sp_raw(5, 0);
	controller_sp.z = controller_sp_raw(6, 0);
	controller_sp.vz = controller_sp_raw(7, 0);

	comm::Controller controller_data;
	controller_data.setpoint = controller_sp;
	controller_data.process_value = process_value;
	controller_data.angles[0] = Actuators::getInstance().getFinAngle(Actuators::Fin::FIN1);
	controller_data.angles[1] = Actuators::getInstance().getFinAngle(Actuators::Fin::FIN2);
	controller_data.angles[2] = Actuators::getInstance().getFinAngle(Actuators::Fin::FIN3);
	controller_data.angles[3] = Actuators::getInstance().getFinAngle(Actuators::Fin::FIN4);
	controller_data.throttle = Actuators::getInstance().getMotorThrottle();
	controller_data.state = current;

	return controller_data;
}
