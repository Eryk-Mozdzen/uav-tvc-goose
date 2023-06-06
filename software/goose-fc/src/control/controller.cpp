#include "controller.h"

Controller::Controller() {

}

void Controller::setSetpoint(const Matrix<X_NUM, 1> sp) {
    setpoint = sp;
}

void Controller::setProcessValue(const Matrix<X_NUM, 1> pv) {
    process_value = pv;
}

Matrix<Controller::U_NUM, 1> Controller::calculate() const {
    const Matrix<X_NUM, 1> error = process_value - setpoint;

    return operating_point - K*error;
}

Matrix<Controller::X_NUM, 1> Controller::getSetpoint() const {
    return setpoint;
}

Matrix<Controller::X_NUM, 1> Controller::getProcessValue() const {
    return process_value;
}

Controller & Controller::getInstance() {
    static Controller controller;

    return controller;
}
