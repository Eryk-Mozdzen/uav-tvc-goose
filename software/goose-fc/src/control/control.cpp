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
#include "memory.h"

class Control : public TaskClassS<2048>, public Context {
	events::Command cmd_start;
	events::Command cmd_land;
	events::Command cmd_abort;
	events::Watchdog disconnect;
	events::Limits limits;
	events::Buffer limits_continous;
	events::Movement movement;
	events::Negation correct;
	events::Negation connect;
	events::Negation stil;
	events::Combination<3> readiness;
	events::Watchdog no_cmd_manual;
	events::Negation cmd_manual;

	states::Abort abort;
	states::Ready ready;
	states::Active active;
	states::Landing landing;
	states::Manual manual;

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
		cmd_abort{comm::Command::ABORT},
		disconnect{1000},
		limits_continous{&limits, 500},
		correct{&limits, 3000},
		connect{&disconnect, 3000},
		stil{&movement, 3000},
		readiness{&correct, &connect, &stil},
		no_cmd_manual{3000},
		cmd_manual{&no_cmd_manual, 3000},
		sm{&abort, this},
		telemetry_controller{"controller telememetry", Transfer::ID::TELEMETRY_CONTROLLER} {

	sm.transit(&abort, &ready, &readiness);

	sm.transit(&ready, &active, &cmd_start);
	sm.transit(&ready, &abort, &limits);
	sm.transit(&ready, &abort, &disconnect);

	sm.transit(&active, &abort, &limits_continous);
	sm.transit(&active, &abort, &cmd_abort);
	sm.transit(&active, &landing, &cmd_land);
	sm.transit(&active, &landing, &disconnect);

	sm.transit(&landing, &abort, &limits_continous);
	sm.transit(&landing, &abort, &cmd_abort);
	sm.transit(&landing, &ready, &stil);

	sm.transit(&ready, &manual, &cmd_manual);
	sm.transit(&manual, &ready, &no_cmd_manual);
}

void Control::task() {

	sm.start();

	TickType_t time = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&time, 10);

		Transfer::FrameRX frame;
		while(Transport::getInstance().frame_rx_queue.pop(frame, 0)) {
			if(frame.id==Transfer::ID::CONTROL_COMMAND) {
				cmd_start.check(frame);
				cmd_land.check(frame);
				cmd_abort.check(frame);

				comm::Command command;
				if(frame.getPayload(command)) {
					if(command==comm::Command::RESET) {
						NVIC_SystemReset();
					}
				}
			}

			if(frame.id==Transfer::ID::CONTROL_SETPOINT) {
				if(frame.getPayload(setpoint)) {
					disconnect.reset();
				}
			}

			if(frame.id==Transfer::ID::CONTROL_MANUAL) {
				no_cmd_manual.reset();
				if(frame.getPayload(control_manual)) {
					disconnect.reset();
				}
			}
		}

		while(Transport::getInstance().state_queue.pop(process_value, 0)) {
			Controller::getInstance().setProcessValue({
				process_value.rpy[0],
				process_value.rpy[1],
				//process_value.rpy[2],
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
	const Matrix<7, 1> controller_sp_raw = Controller::getInstance().getSetpoint();
	comm::Controller::State controller_sp;
	controller_sp.rpy[0] = controller_sp_raw(0, 0);
	controller_sp.rpy[1] = controller_sp_raw(1, 0);
	//controller_sp.rpy[2] = controller_sp_raw(2, 0);
	controller_sp.rpy[2] = 0;
	controller_sp.w[0] = controller_sp_raw(2, 0);
	controller_sp.w[1] = controller_sp_raw(3, 0);
	controller_sp.w[2] = controller_sp_raw(4, 0);
	controller_sp.z = controller_sp_raw(5, 0);
	controller_sp.vz = controller_sp_raw(6, 0);

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
