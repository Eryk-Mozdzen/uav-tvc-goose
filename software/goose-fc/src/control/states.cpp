#include "states.h"
#include "logger.h"

namespace states {

static comm::Controller::SMState current;

comm::Controller::SMState getCurrent() {
    return current;
}

void Abort::enter() {
    current = comm::Controller::SMState::ABORT;
    Logger::getInstance().log(Logger::ERROR, "sm: aborting");
}

void Ready::enter() {
    current = comm::Controller::SMState::READY;
    Logger::getInstance().log(Logger::INFO, "sm: ready to launch");
}

void Active::enter() {
    current = comm::Controller::SMState::ACTIVE;
    Logger::getInstance().log(Logger::INFO, "sm: ready to fly");
}

void Active::execute() {

}

void TakeOff::enter() {
    current = comm::Controller::SMState::TAKE_OFF;
    Logger::getInstance().log(Logger::INFO, "sm: starting...");
}

void TakeOff::execute() {

}

void Landing::enter() {
    current = comm::Controller::SMState::LANDING;
    Logger::getInstance().log(Logger::INFO, "sm: landing...");
}

void Landing::execute() {

}

}