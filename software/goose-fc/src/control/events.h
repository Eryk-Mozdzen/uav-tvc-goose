#pragma once

#include "event.h"
#include "transfer.h"
#include "TimerCPP.h"
#include "quaternion.h"

namespace events {

class Command : public sm::Event {
    const Transfer::ID id;

public:
    Command(const Transfer::ID id);
    void feed(const Transfer::FrameRX &frame);
};

class Watchdog : public sm::Event {
    TimerMember<Watchdog> timer;

    void timeout();

public:
    Watchdog(const TickType_t period);
    void reset();
};

class StateLimit : public sm::Event {
    const float max_angle;
    const float max_altitude;

public:
    StateLimit(const float angle_deg, const float alt);
    void check(const Quaternion attitude, const float altitude);
};

}
