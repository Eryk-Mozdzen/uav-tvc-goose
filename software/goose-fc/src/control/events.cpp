#include "events.h"
#include "logger.h"

namespace events {

Command::Command(const Transfer::Command cmd) : cmd{cmd} {

}

void Command::feed(const Transfer::FrameRX &frame) {
    if(frame.id==Transfer::ID::CONTROL_COMMAND) {
        Transfer::Command command;

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

StateLimit::StateLimit(const float angle_deg, const float alt) : max_angle{angle_deg*3.1415f/180.f}, max_altitude{alt} {

}

void StateLimit::check(const Quaternion attitude, const float altitude) {
    const Matrix<3, 1> rpy = attitude.getRollPitchYaw();
    const float roll = rpy(0, 0);
    const float pitch = rpy(1, 0);

    const float curr_cos = fabs(cosf(roll)*cosf(pitch));
    const float max_cos = fabs(cosf(max_angle));

    if(curr_cos>max_cos || altitude>max_altitude) {
        Logger::getInstance().log(Logger::INFO, "sm: state limits exceeded");
        sm::Event::trigger();
    }
}

}
