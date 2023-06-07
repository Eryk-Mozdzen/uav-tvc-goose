#pragma once

#include "comm.h"

struct Context {
    comm::Controller::SMState current;
	comm::Controller::State setpoint;
	comm::Controller::State process_value;

    Context() : current{comm::Controller::SMState::ABORT} {}
};
