#include "events.h"
#include "logger.h"
#include "states.h"

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

void Negation::reset() {
    timer.reset();
    repeat = false;
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
        sm::Event::trigger();
    }
}

Movement::Movement(const float w_thres, const float v_thres) :
        angular_velocity_threshold{w_thres*deg2rad},
        linear_velocity_threshold{v_thres} {

}

void Movement::check(const comm::Controller::State &state) {
    const float Wx = fabs(state.w[0]);
    const float Wy = fabs(state.w[1]);
    const float Wz = fabs(state.w[2]);
    const float Vz = fabs(state.vz);

    if(Wx>angular_velocity_threshold || Wy>angular_velocity_threshold || Wz>angular_velocity_threshold || Vz>linear_velocity_threshold) {
        sm::Event::trigger();
    }
}

AltitudeReached::AltitudeReached(const float des, const float marg, const TickType_t period) :
        desired{des},
        margin{marg},
        timer{"altitude reached timer", this, &AltitudeReached::callback, period, pdTRUE} {

    timer.start();
}

void AltitudeReached::check(const comm::Controller::State &state) {
    const float altitude = state.z;

    if(std::abs(altitude - desired)>margin) {
        timer.reset();
    }
}

void AltitudeReached::callback() {
    sm::Event::trigger();
}

}
