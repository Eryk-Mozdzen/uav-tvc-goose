#include "events.h"

namespace events {

Command::Command(const comm::Command cmd) : cmd{cmd}, flag{false} {

}

void Command::check(const Transfer::FrameRX &frame) {
    if(frame.id!=Transfer::ID::CONTROL_COMMAND) {
        return;
    }

    comm::Command command;
    if(!frame.getPayload(command)) {
        return;
    }

    if(command!=cmd) {
        return;
    }

    flag = true;
}

bool Command::triggered() {
    return flag;
}

void Command::clear() {
    flag = false;
}

void Command::action() {
    Logger::getInstance().log(Logger::DEBUG, "ev: Command %d received", cmd);
}

Watchdog::Watchdog(const TickType_t period) :
        timer{"event watchdog", this, &Watchdog::timeout, period, pdTRUE} {

    timer.start();
}

void Watchdog::reset() {
    timer.reset();
    flag = false;
}

void Watchdog::timeout() {
    flag = true;
}

bool Watchdog::triggered() {
    return flag;
}

void Watchdog::action() {
    Logger::getInstance().log(Logger::DEBUG, "ev: Watchdog timeout");
}


Buffer::Buffer(sm::Event<Context> *target, const TickType_t period) :
        target{target},
        timer{"buffer timer", this, &Buffer::callback, period, pdFALSE},
        repeat{false} {

    timer.start();
}

bool Buffer::triggered() {
    if(!target->triggered()) {
        timer.reset();
        repeat = false;
    }

    return repeat;
}

void Buffer::callback() {
    repeat = true;
}

void Buffer::reset() {
    timer.reset();
    repeat = false;
}

void Buffer::action() {
    target->action();
}

Negation::Negation(sm::Event<Context> *target, const TickType_t period) :
        target{target},
        timer{"negation timer", this, &Negation::callback, period, pdFALSE},
        repeat{false} {

    timer.start();
}

bool Negation::triggered() {
    if(target->triggered()) {
        timer.reset();
        repeat = false;
    }

    return repeat;
}

void Negation::callback() {
    repeat = true;
}

void Negation::reset() {
    timer.reset();
    repeat = false;
}

void Negation::action() {
    Logger::getInstance().log(Logger::DEBUG, "ev: Negation occure");
}

bool Limits::triggered() {
    const float roll = context->process_value.rpy[0];
    const float pitch = context->process_value.rpy[1];
    const float altitude = context->process_value.z;

    const float curr_cos = fabs(cosf(roll)*cosf(pitch));
    const float max_cos = fabs(cosf(max_angle));

    return (curr_cos<max_cos || altitude>max_altitude);
}

void Limits::action() {
    const float roll = context->process_value.rpy[0];
    const float pitch = context->process_value.rpy[1];
    const float altitude = context->process_value.z;

    const float curr_cos = fabs(cosf(roll)*cosf(pitch));
    const float max_cos = fabs(cosf(max_angle));

    if(curr_cos<max_cos) {
        Logger::getInstance().log(
            Logger::DEBUG,
            "ev: Attitude exceeds limits: roll: %+3.0f deg, pitch: %+3.0f deg",
            (double)(roll/deg2rad),
            (double)(pitch/deg2rad));
    }

    if(altitude>max_altitude) {
        Logger::getInstance().log(Logger::DEBUG, "ev: Altitude exceeds limits: %+3.3f m", (double)altitude);
    }
}

bool Movement::triggered() {
    const float Wx = fabs(context->process_value.w[0]);
    const float Wy = fabs(context->process_value.w[1]);
    const float Wz = fabs(context->process_value.w[2]);
    const float Vz = fabs(context->process_value.vz);

    return (Wx>angular_velocity_threshold || Wy>angular_velocity_threshold || Wz>angular_velocity_threshold || Vz>linear_velocity_threshold);
}

void Movement::action() {
    Logger::getInstance().log(Logger::DEBUG, "ev: Movement detected");
}

}
