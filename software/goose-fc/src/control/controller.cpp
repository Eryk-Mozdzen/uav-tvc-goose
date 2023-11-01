#include "controller.h"

Controller::Controller() {

}

Controller & Controller::getInstance() {
    static Controller controller;

    return controller;
}

comm::Controller::State Controller::matrix2state(const Matrix<controller::dimX, 1> &matrix) {
    comm::Controller::State state;
	state.rpy[0] = matrix(0, 0);
	state.rpy[1] = matrix(1, 0);
	state.rpy[2] = 0;
	state.w[0] = matrix(2, 0);
	state.w[1] = matrix(3, 0);
	state.w[2] = matrix(4, 0);
	state.z = matrix(5, 0);
	state.vz = matrix(6, 0);
    return state;
}

Matrix<controller::dimX, 1> Controller::state2matrix(const comm::Controller::State &state) {
    const Matrix<controller::dimX, 1> matrix = {
        state.rpy[0],
        state.rpy[1],
        state.w[0],
        state.w[1],
        state.w[2],
        state.z,
        state.vz
    };
    return matrix;
}

void Controller::reset() {
    for(int i=0; i<controller::dimG; i++) {
        error_integral(i, 0) = 0.f;
    }
}

Matrix<controller::dimU, 1> Controller::calculate(const comm::Controller::State &pv) {
    process_value = state2matrix(pv);

    const Matrix<controller::dimX, 1> error = process_value - setpoint;

    error_integral +=controller::G*error*dt;

    Matrix<controller::dimX + controller::dimG, 1> e;

    for(int i=0; i<controller::dimX; i++) {
        e(i, 0) = error(i, 0);
    }

    for(int i=0; i<controller::dimG; i++) {
        e(controller::dimX + i, 0) = error_integral(i, 0);
    }

    return controller::operating_point - controller::K*e;
}

void Controller::setSetpoint(const comm::Controller::State &sp) {
    setpoint = state2matrix(sp);
}

const comm::Controller::State Controller::getSetpoint() const {
    return matrix2state(setpoint);
}

const comm::Controller::State Controller::getProcessValue() const {
    return matrix2state(process_value);
}
