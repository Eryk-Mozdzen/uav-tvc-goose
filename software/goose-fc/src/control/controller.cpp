#include "controller.h"

Controller::Controller() {

}

void Controller::setSetpoint(const Matrix<X_NUM, 1> sp) {
    setpoint = sp;
}

void Controller::setProcessValue(const Matrix<X_NUM, 1> pv) {
    process_value = pv;
}

Matrix<Controller::U_NUM, 1> Controller::calculate() {
    const Matrix<X_NUM, 1> error = process_value - setpoint;

    error_integral +=G*error*dt;

    Matrix<X_NUM + G_NUM, 1> e;
    for(int i=0; i<X_NUM; i++) {
        e(i, 0) = error(i, 0);
    }
    for(int i=0; i<G_NUM; i++) {
        e(i + X_NUM, 0) = error_integral(i, 0);
    }

    return operating_point - K*e;
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
