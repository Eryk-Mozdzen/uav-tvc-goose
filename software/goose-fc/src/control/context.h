#pragma once

#include "comm.h"
#include "matrix.h"

struct Context {
    comm::Controller::SMState current;
	comm::Controller::State setpoint;
	comm::Controller::State process_value;
	comm::Manual control_manual;
	Matrix<5, 1> control_feedback;

    Context() : current{comm::Controller::SMState::ABORT} {}
};
