#include "events.h"
#include "logger.h"

namespace events {

Command::Command(const comm::Command cmd) : cmd{cmd} {

}

void Command::feed(const Transfer::FrameRX &frame) {
    if(frame.id==Transfer::ID::CONTROL_COMMAND) {
        comm::Command command;

        if(frame.getPayload(command)) {
            if(command==cmd) {
                Logger::getInstance().log(Logger::INFO, "sm: command %d received", cmd);
                sm::Event::trigger();
            }
        }
    }
}

Watchdog::Watchdog(const TickType_t period) :
        timer{"event watchdog", this, &Watchdog::timeout, period, pdTRUE} {

    timer.start();
}

void Watchdog::reset() {
    timer.reset();
}

void Watchdog::timeout() {
    Logger::getInstance().log(Logger::INFO, "sm: watchdog timeout");
    sm::Event::trigger();
}

Negation::Negation(const sm::Event *target, const TickType_t period) :
        target{target},
        timer{"negation timer", this, &Negation::callback, period, pdFALSE},
        repeat{false} {

    timer.start();
}

void Negation::check() {
    if(target->isTriggered()) {
        timer.reset();
        repeat = false;
        return;
    }

    if(repeat) {
        sm::Event::trigger();
        return;
    }
}

void Negation::callback() {
    sm::Event::trigger();
    repeat = true;
}

StateLimits::StateLimits(const float angle_deg, const float alt) : max_angle{angle_deg*deg2rad}, max_altitude{alt} {

}

void StateLimits::check(const comm::Controller::State &state) {
    const float roll = state.rpy[0];
    const float pitch = state.rpy[1];
    const float altitude = state.z;

    const float curr_cos = fabs(cosf(roll)*cosf(pitch));
    const float max_cos = fabs(cosf(max_angle));

    if(curr_cos<max_cos || altitude>max_altitude) {
        //Logger::getInstance().log(Logger::WARNING, "sm: state limits exceeded");
        sm::Event::trigger();
    }
}

}
