#include "states.h"
#include "logger.h"
#include "actuators.h"
#include "controller.h"

namespace states {

static comm::Controller::SMState current;

comm::Controller::SMState getCurrent() {
    return current;
}

void Abort::enter() {
    current = comm::Controller::SMState::ABORT;
    Logger::getInstance().log(Logger::ERROR, "sm: aborting");

    Controller::getInstance().setSetpoint({0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f});

    Actuators::getInstace().setFinAngle(Actuators::Fin::FIN1, 0.f);
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN2, 0.f);
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN3, 0.f);
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN4, 0.f);
	Actuators::getInstace().setMotorThrottle(0.f);
}

void Ready::enter() {
    current = comm::Controller::SMState::READY;
    Logger::getInstance().log(Logger::INFO, "sm: ready to launch");

    Controller::getInstance().setSetpoint({0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f});

    Actuators::getInstace().setFinAngle(Actuators::Fin::FIN1, 0.f);
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN2, 0.f);
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN3, 0.f);
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN4, 0.f);
	Actuators::getInstace().setMotorThrottle(0.f);
}

Active::Active(const comm::Controller::State &sp) : setpoint{sp} {

}

void Active::enter() {
    current = comm::Controller::SMState::ACTIVE;
    Logger::getInstance().log(Logger::INFO, "sm: ready to fly");
}

void Active::execute() {
    Controller::getInstance().setSetpoint({
        setpoint.rpy[0],
        setpoint.rpy[1],
        setpoint.rpy[2],
        setpoint.w[0],
        setpoint.w[1],
        setpoint.w[2],
        setpoint.z,
        setpoint.vz
    });

    const Matrix<5, 1> u = Controller::getInstance().calculate();

    Actuators::getInstace().setFinAngle(Actuators::Fin::FIN1, u(0, 0));
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN2, u(1, 0));
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN3, u(2, 0));
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN4, u(3, 0));
	Actuators::getInstace().setMotorThrottle(u(4, 0));
}

TakeOff::TakeOff(events::Negation &event) : event{event} {

}

void TakeOff::enter() {
    current = comm::Controller::SMState::TAKE_OFF;
    Logger::getInstance().log(Logger::INFO, "sm: starting...");

    event.reset();
}

void TakeOff::execute() {
    const Matrix<5, 1> u = Controller::getInstance().calculate();

    Actuators::getInstace().setFinAngle(Actuators::Fin::FIN1, u(0, 0));
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN2, u(1, 0));
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN3, u(2, 0));
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN4, u(3, 0));
	Actuators::getInstace().setMotorThrottle(u(4, 0));
}

Landing::Landing(events::Negation &event, const comm::Controller::State &pv) : event{event}, process_value{pv} {

}

void Landing::enter() {
    current = comm::Controller::SMState::LANDING;
    Logger::getInstance().log(Logger::INFO, "sm: landing...");

    event.reset();

    sp_altitude = process_value.z;
}

void Landing::execute() {
    sp_altitude -=decrement;

    Controller::getInstance().setSetpoint({
        0.f,
        0.f,
        0.f,
        0.f,
        0.f,
        0.f,
        sp_altitude,
        0.f
    });

    const Matrix<5, 1> u = Controller::getInstance().calculate();

    Actuators::getInstace().setFinAngle(Actuators::Fin::FIN1, u(0, 0));
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN2, u(1, 0));
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN3, u(2, 0));
	Actuators::getInstace().setFinAngle(Actuators::Fin::FIN4, u(3, 0));
	Actuators::getInstace().setMotorThrottle(u(4, 0));
}

}