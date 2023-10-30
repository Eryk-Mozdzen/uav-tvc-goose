#include "states.h"
#include "logger.h"
#include "actuators.h"
#include "controller.h"

namespace states {

void Abort::enter() {
    context->current = comm::Controller::SMState::ABORT;
    Logger::getInstance().log(Logger::ERROR, "sm: aborting");

    Controller::getInstance().setSetpoint({0.f, 0.f, /*0.f,*/ 0.f, 0.f, 0.f, 0.f, 0.f});

    Actuators::getInstance().setFinAngle(Actuators::Fin::FIN1, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN2, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN3, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN4, 0);
	Actuators::getInstance().setMotorThrottle(0, Actuators::Mode::PASSTHROUGH);
}

void Ready::enter() {
    context->current = comm::Controller::SMState::READY;
    Logger::getInstance().log(Logger::INFO, "sm: ready to launch");

    Controller::getInstance().setSetpoint({0.f, 0.f, /*0.f,*/ 0.f, 0.f, 0.f, 0.f, 0.f});

    Actuators::getInstance().setFinAngle(Actuators::Fin::FIN1, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN2, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN3, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN4, 0);
	Actuators::getInstance().setMotorThrottle(0, Actuators::Mode::RAMP);
}

void Ready::execute() {
	Actuators::getInstance().setMotorThrottle(0, Actuators::Mode::RAMP);
}

void Active::enter() {
    context->current = comm::Controller::SMState::ACTIVE;
    Controller::getInstance().resetIntegral();
    Logger::getInstance().log(Logger::INFO, "sm: ready to fly");
}

void Active::execute() {
    Controller::getInstance().setSetpoint({
        context->setpoint.rpy[0],
        context->setpoint.rpy[1],
        //context->setpoint.rpy[2],
        context->setpoint.w[0],
        context->setpoint.w[1],
        context->setpoint.w[2],
        context->setpoint.z,
        context->setpoint.vz
    });

    const Matrix<5, 1> u = Controller::getInstance().calculate();

    Actuators::getInstance().setFinAngle(Actuators::Fin::FIN1, u(0, 0));
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN2, u(1, 0));
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN3, u(2, 0));
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN4, u(3, 0));
	Actuators::getInstance().setMotorThrottle(u(4, 0), Actuators::Mode::RAMP);
}

void Landing::enter() {
    context->current = comm::Controller::SMState::LANDING;
    Logger::getInstance().log(Logger::INFO, "sm: landing...");

    sp_altitude = context->process_value.z;
}

void Landing::execute() {
    sp_altitude -=decrement;

    Controller::getInstance().setSetpoint({
        0.f,
        0.f,
        //0.f,
        0.f,
        0.f,
        0.f,
        sp_altitude,
        0.f
    });

    const Matrix<5, 1> u = Controller::getInstance().calculate();

    Actuators::getInstance().setFinAngle(Actuators::Fin::FIN1, u(0, 0));
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN2, u(1, 0));
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN3, u(2, 0));
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN4, u(3, 0));
	Actuators::getInstance().setMotorThrottle(u(4, 0), Actuators::Mode::RAMP);
}

void Manual::enter() {
    context->current = comm::Controller::SMState::MANUAL;
    Logger::getInstance().log(Logger::INFO, "sm: manual mode");

    Actuators::getInstance().setFinAngle(Actuators::Fin::FIN1, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN2, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN3, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN4, 0);
	Actuators::getInstance().setMotorThrottle(0, Actuators::Mode::RAMP);
}

void Manual::execute() {

    if(context->control_manual.type==comm::Manual::Type::NORMAL) {
        Actuators::getInstance().setMotorThrottle(context->control_manual.throttle, Actuators::Mode::RAMP);
        Actuators::getInstance().setFinAngle(Actuators::FIN1, context->control_manual.angles[0]);
        Actuators::getInstance().setFinAngle(Actuators::FIN2, context->control_manual.angles[1]);
        Actuators::getInstance().setFinAngle(Actuators::FIN3, context->control_manual.angles[2]);
        Actuators::getInstance().setFinAngle(Actuators::FIN4, context->control_manual.angles[3]);
    } else {
        Actuators::getInstance().setFinCompare(Actuators::FIN1, context->control_manual.angles[0]);
        Actuators::getInstance().setFinCompare(Actuators::FIN2, context->control_manual.angles[1]);
        Actuators::getInstance().setFinCompare(Actuators::FIN3, context->control_manual.angles[2]);
        Actuators::getInstance().setFinCompare(Actuators::FIN4, context->control_manual.angles[3]);
    }
}

void Manual::exit() {
    context->current = comm::Controller::SMState::MANUAL;
    Logger::getInstance().log(Logger::INFO, "sm: auto mode");

    Actuators::getInstance().setFinAngle(Actuators::Fin::FIN1, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN2, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN3, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN4, 0);
	Actuators::getInstance().setMotorThrottle(0, Actuators::Mode::RAMP);
}

}