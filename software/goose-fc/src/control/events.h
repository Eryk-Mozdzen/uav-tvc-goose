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

class Negation : public sm::Event {
    const sm::Event *target;
    TimerMember<Negation> timer;
    bool repeat;

    void callback();

public:
    Negation(const sm::Event *target, const TickType_t period);
    void check();
    void reset();
};

template<int N>
class Combination : public sm::Event {
    sm::Event *targets[N];

    void on_clear() override {
        for(int i=0; i<N; i++) {
            targets[i]->clear();
        }
    }

public:
    template<typename... U>
    Combination(U... list) : targets{static_cast<sm::Event *>(list)...} {
        static_assert(sizeof...(list)==N, "wrong num of targets");
    }

    void check() {
        for(int i=0; i<N; i++) {
            if(!targets[i]->isTriggered()) {
                return;
            }
        }

        sm::Event::trigger();
    }
};

class StateLimits : public sm::Event {
    static constexpr float deg2rad = 3.1415f/180.f;
    const float max_angle;
    const float max_altitude;

public:
    StateLimits(const float angle_deg, const float alt);
    void check(const comm::Controller::State &state);
};

class Movement : public sm::Event {
    static constexpr float deg2rad = 3.1415f/180.f;
    const float angular_velocity_threshold;
    const float linear_velocity_threshold;

public:
    Movement(const float w_thres, const float v_thres);
    void check(const comm::Controller::State &state);
};

class AltitudeReached : public sm::Event {
    const float desired;
    const float margin;
    TimerMember<AltitudeReached> timer;

    void callback();

public:
    AltitudeReached(const float des, const float marg, const TickType_t period);
    void check(const comm::Controller::State &state);
};

}
