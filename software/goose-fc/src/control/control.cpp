#include "TaskCPP.h"

#include "transport.h"
#include "transfer.h"

#include "state_machine.h"
#include "state_abort.h"
#include "state_ready.h"
#include "state_active.h"
#include "state_takeoff.h"
#include "state_landing.h"

class Control : public TaskClassS<2048> {
	StateAbort abort;
	StateReady ready;
	StateActive active;
	StateTakeOff takeoff;
	StateLanding landing;

	sm::StateMachine<5, 4> sm;

public:
	Control();

	void task();
};

Control control;

Control::Control() : TaskClassS{"control loop", TaskPrio_Mid},
		abort{this},
		ready{this},
		active{this},
		takeoff{this},
		landing{this},
		sm{&abort} {

	sm.transit(&abort, &ready, nullptr);
	sm.transit(&ready, &abort, nullptr);
	sm.transit(&ready, &abort, nullptr);
	sm.transit(&ready, &abort, nullptr);
	sm.transit(&ready, &takeoff, nullptr);

	sm.transit(&takeoff, &abort, nullptr);
	sm.transit(&takeoff, &active, nullptr);
	sm.transit(&takeoff, &landing, nullptr);
	sm.transit(&takeoff, &landing, nullptr);

	sm.transit(&active, &abort, nullptr);
	sm.transit(&active, &landing, nullptr);
	sm.transit(&active, &landing, nullptr);

	sm.transit(&landing, &abort, nullptr);
	sm.transit(&landing, &ready, nullptr);
}

void Control::task() {

	Transfer::FrameRX frame;

	while(1) {
		Transport::getInstance().frame_rx_queue.pop(frame, portMAX_DELAY);
	}
}
