#pragma once

#include "comm.h"

struct Context {
    comm::Controller::SMState current;
	comm::Controller::State setpoint;
	comm::Controller::State process_value;
	comm::Manual control_manual;

    Context() : current{comm::Controller::SMState::ABORT} {}
};
