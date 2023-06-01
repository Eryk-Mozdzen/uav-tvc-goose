#include "states.h"
#include "logger.h"

namespace states {

void Abort::enter() {
    Logger::getInstance().log(Logger::INFO, "sm: aborting");
}

void Ready::enter() {
    Logger::getInstance().log(Logger::INFO, "sm: ready to launch");
}

void Active::enter() {
    Logger::getInstance().log(Logger::INFO, "sm: waiting for commands");
}

void Active::execute() {

}

void TakeOff::enter() {
    Logger::getInstance().log(Logger::INFO, "sm: starting...");
}

void TakeOff::execute() {

}

void Landing::enter() {
    Logger::getInstance().log(Logger::INFO, "sm: landing...");
}

void Landing::execute() {

}

}