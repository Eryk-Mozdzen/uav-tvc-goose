#pragma once

#include "event.h"
#include "transfer.h"
#include "TimerCPP.h"
#include "quaternion.h"
#include "comm.h"

namespace events {

class Command : public sm::Event {
    const comm::Command cmd;

public:
    Command(const comm::Command cmd);
    void feed(const Transfer::FrameRX &frame);
};

class Watchdog : public sm::Event {
    TimerMember<Watchdog> timer;

    void timeout();

public:
    Watchdog(const TickType_t period);
    void reset();
};

class Negator : public sm::Event {
    const sm::Event *target;
    TimerMember<Negator> timer;
    bool repeat;

    void callback();

public:
    Negator(const sm::Event *target, const TickType_t period);
    void check();
};

class StateLimits : public sm::Event {
    static constexpr float deg2rad = 3.1415f/180.f;
    const float max_angle;
    const float max_altitude;

public:
    StateLimits(const float angle_deg, const float alt);
    void check(const comm::Controller::State &state);
};

}
