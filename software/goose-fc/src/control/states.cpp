#include "states.h"
#include "logger.h"
#include "actuators.h"
#include "controller.h"
#include "lights.h"

namespace states {

void Abort::enter() {
    context->current = comm::Controller::SMState::ABORT;
    Logger::getInstance().log(Logger::ERROR, "sm: aborting");

    Lights::set(Lights::Red, true);
}

void Abort::execute() {
    Actuators::getInstance().setFinAngle(Actuators::Fin::FIN1, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN2, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN3, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN4, 0);
	Actuators::getInstance().setMotorThrottle(0, Actuators::Mode::PASSTHROUGH);
}

void Abort::exit() {
    Lights::set(Lights::Red, false);
}

void Ready::enter() {
    context->current = comm::Controller::SMState::READY;
    Logger::getInstance().log(Logger::INFO, "sm: ready to launch");

    Lights::set(Lights::Green, true);
}

void Ready::execute() {
    Actuators::getInstance().setFinAngle(Actuators::Fin::FIN1, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN2, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN3, 0);
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN4, 0);
	Actuators::getInstance().setMotorThrottle(0, Actuators::Mode::RAMP);
}

void Ready::exit() {
    Lights::set(Lights::Green, false);
}

void Active::enter() {
    context->current = comm::Controller::SMState::ACTIVE;
    Logger::getInstance().log(Logger::INFO, "sm: ready to fly");

    Controller::getInstance().reset();
}

void Active::execute() {
    Controller::getInstance().setSetpoint(context->setpoint);

    Actuators::getInstance().setFinAngle(Actuators::Fin::FIN1, context->control_feedback(0, 0));
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN2, context->control_feedback(1, 0));
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN3, context->control_feedback(2, 0));
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN4, context->control_feedback(3, 0));
	Actuators::getInstance().setMotorThrottle(context->control_feedback(4, 0), Actuators::Mode::RAMP);
}

void Active::exit() {
    Controller::getInstance().setSetpoint(comm::Controller::State::zero());
}

void Landing::enter() {
    context->current = comm::Controller::SMState::LANDING;
    Logger::getInstance().log(Logger::INFO, "sm: landing...");

    setpoint = comm::Controller::State::zero();
    setpoint.z = Controller::getInstance().getProcessValue().z;
}

void Landing::execute() {
    setpoint.z -=decrement;

    Controller::getInstance().setSetpoint(setpoint);

    Actuators::getInstance().setFinAngle(Actuators::Fin::FIN1, context->control_feedback(0, 0));
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN2, context->control_feedback(1, 0));
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN3, context->control_feedback(2, 0));
	Actuators::getInstance().setFinAngle(Actuators::Fin::FIN4, context->control_feedback(3, 0));
	Actuators::getInstance().setMotorThrottle(context->control_feedback(4, 0), Actuators::Mode::RAMP);
}

void Landing::exit() {
    Controller::getInstance().setSetpoint(comm::Controller::State::zero());
}

void Manual::enter() {
    context->current = comm::Controller::SMState::MANUAL;
    Logger::getInstance().log(Logger::INFO, "sm: manual mode");
}

void Manual::execute() {
    if(context->control_manual.type==comm::Manual::Type::NORMAL) {
        Actuators::getInstance().setFinAngle(Actuators::FIN1, context->control_manual.angles[0]);
        Actuators::getInstance().setFinAngle(Actuators::FIN2, context->control_manual.angles[1]);
        Actuators::getInstance().setFinAngle(Actuators::FIN3, context->control_manual.angles[2]);
        Actuators::getInstance().setFinAngle(Actuators::FIN4, context->control_manual.angles[3]);
        Actuators::getInstance().setMotorThrottle(context->control_manual.throttle, Actuators::Mode::RAMP);
    } else {
        Actuators::getInstance().setFinCompare(Actuators::FIN1, context->control_manual.angles[0]);
        Actuators::getInstance().setFinCompare(Actuators::FIN2, context->control_manual.angles[1]);
        Actuators::getInstance().setFinCompare(Actuators::FIN3, context->control_manual.angles[2]);
        Actuators::getInstance().setFinCompare(Actuators::FIN4, context->control_manual.angles[3]);
    }
}

}
