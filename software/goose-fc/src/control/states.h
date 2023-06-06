#pragma once

#include "state.h"
#include "comm.h"
#include "events.h"

namespace states {

comm::Controller::SMState getCurrent();

class Abort : public sm::State {
    void enter() override;
};

class Ready : public sm::State {
    void enter() override;
};

class Active : public sm::State {
    const comm::Controller::State &setpoint;

    void enter() override;
    void execute() override;

public:
    Active(const comm::Controller::State &sp);
};

class TakeOff : public sm::State {
    events::Negation &event;

    void enter() override;
    void execute() override;

public:
    TakeOff(events::Negation &event);
};

class Landing : public sm::State {
    events::Negation &event;
    const comm::Controller::State &process_value;
    float sp_altitude;

    static constexpr float freq = 100.f;
    static constexpr float decent_vel = 0.f;
    static constexpr float decrement = 0.1f/freq;

    void enter() override;
    void execute() override;

public:
    Landing(events::Negation &event, const comm::Controller::State &pv);
};

}
