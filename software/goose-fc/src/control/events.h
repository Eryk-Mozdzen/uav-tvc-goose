#pragma once

#include "event.h"
#include "transfer.h"
#include "TimerCPP.h"
#include "comm.h"
#include "context.h"

namespace events {

class Command : public sm::Event<Context> {
    const comm::Command cmd;
    bool flag;

public:
    Command(const comm::Command cmd);
    void check(const Transfer::FrameRX &frame);
    bool triggered();
    void clear() override;
};

class Watchdog : public sm::Event<Context> {
    TimerMember<Watchdog> timer;
    bool flag;

    void timeout();

public:
    Watchdog(const TickType_t period);
    bool triggered();
    void reset() override;
};

class Negation : public sm::Event<Context> {
    sm::Event<Context> *target;
    TimerMember<Negation> timer;
    bool repeat;

    void callback();

public:
    Negation(sm::Event<Context> *target, const TickType_t period);
    bool triggered();
    void reset() override;
};

template<int N>
class Combination : public sm::Event<Context> {
    sm::Event<Context> *targets[N];

public:
    template<typename... U>
    Combination(U... list) : targets{static_cast<sm::Event<Context> *>(list)...} {
        static_assert(sizeof...(list)==N, "wrong num of targets");
    }

    bool triggered() {
        for(int i=0; i<N; i++) {
            if(!targets[i]->triggered()) {
                return false;
            }
        }

        return true;
    }
};

class Limits : public sm::Event<Context> {
    static constexpr float deg2rad = 3.1415f/180.f;
    static constexpr float max_angle = 30.f*deg2rad;
    static constexpr float max_altitude = 2.f;

public:
    bool triggered();
};

class Movement : public sm::Event<Context> {
    static constexpr float deg2rad = 3.1415f/180.f;
    static constexpr float angular_velocity_threshold = 10.f*deg2rad;
    static constexpr float linear_velocity_threshold = 0.1f;

public:
    bool triggered();
};

}
