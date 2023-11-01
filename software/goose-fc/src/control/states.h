#pragma once

#include "state.h"
#include "comm.h"
#include "events.h"
#include "context.h"

namespace states {

class Abort : public sm::State<Context> {
public:
    void enter() override;
    void execute() override;
    void exit() override;
};

class Ready : public sm::State<Context> {
public:
    void enter() override;
    void execute() override;
    void exit() override;
};

class Active : public sm::State<Context> {
public:
    void enter() override;
    void execute() override;
    void exit() override;
};

class Landing : public sm::State<Context> {
    static constexpr float freq = 100.f;
    static constexpr float vel = 0.1f;
    static constexpr float decrement = vel/freq;
    comm::Controller::State setpoint;

public:
    void enter() override;
    void execute() override;
    void exit() override;
};

class Manual : public sm::State<Context> {
public:
    void enter() override;
    void execute() override;
};

}
