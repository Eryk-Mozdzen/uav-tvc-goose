#pragma once

#include "matrix.h"
#include "controller_params.h"
#include "comm.h"

class Controller {
    static constexpr float dt = 0.005f;

    Matrix<controller::dimG, 1> error_integral;
    Matrix<controller::dimX, 1> setpoint;
    Matrix<controller::dimX, 1> process_value;

    Controller();
    static comm::Controller::State matrix2state(const Matrix<controller::dimX, 1> &matrix);
    static Matrix<controller::dimX, 1> state2matrix(const comm::Controller::State &state);

public:
    Controller(Controller &) = delete;
	void operator=(const Controller &) = delete;
    static Controller & getInstance();

    void reset();
    Matrix<controller::dimU, 1> calculate(const comm::Controller::State &pv);

    void setSetpoint(const comm::Controller::State &sp);
    const comm::Controller::State getSetpoint() const;
    const comm::Controller::State getProcessValue() const;
};
