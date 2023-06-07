#include "events.h"
#include "logger.h"

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

    Logger::getInstance().log(Logger::INFO, "sm: command %d received", cmd);
    flag = true;
}

bool Command::triggered() {
    return flag;
}

void Command::clear() {
    flag = false;
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
    Logger::getInstance().log(Logger::INFO, "sm: watchdog timeout");
    flag = true;
}

bool Watchdog::triggered() {
    return flag;
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

bool Limits::triggered() {
    const float roll = context->process_value.rpy[0];
    const float pitch = context->process_value.rpy[1];
    const float altitude = context->process_value.z;

    const float curr_cos = fabs(cosf(roll)*cosf(pitch));
    const float max_cos = fabs(cosf(max_angle));

    return (curr_cos<max_cos || altitude>max_altitude);
}

bool Movement::triggered() {
    const float Wx = fabs(context->process_value.w[0]);
    const float Wy = fabs(context->process_value.w[1]);
    const float Wz = fabs(context->process_value.w[2]);
    const float Vz = fabs(context->process_value.vz);

    return (Wx>angular_velocity_threshold || Wy>angular_velocity_threshold || Wz>angular_velocity_threshold || Vz>linear_velocity_threshold);
}

Altitude::Altitude() :
        timer{"altitude timer", this, &Altitude::callback, period, pdTRUE},
        flag{false} {

    timer.start();
}

bool Altitude::triggered() {
    const float altitude = context->process_value.z;

    if(std::abs(altitude - desired)>margin) {
        flag = false;
        timer.reset();
    }

    return flag;
}

void Altitude::callback() {
    flag = true;
}

}
