#pragma once

#include "state.h"
#include "comm.h"
#include "events.h"
#include "context.h"

namespace states {

class Abort : public sm::State<Context> {
public:
    void enter() override;
};

class Ready : public sm::State<Context> {
public:
    void enter() override;
};

class Active : public sm::State<Context> {
public:
    void enter() override;
    void execute() override;
};

class TakeOff : public sm::State<Context> {
    static constexpr float freq = 100.f;
    static constexpr float vel = 0.3f;
    static constexpr float increment = vel/freq;
    static constexpr float limit = 1.5f;
    float sp_altitude;

public:
    void enter() override;
    void execute() override;
};

class Landing : public sm::State<Context> {
    static constexpr float freq = 100.f;
    static constexpr float vel = 0.1f;
    static constexpr float decrement = vel/freq;
    float sp_altitude;

public:
    void enter() override;
    void execute() override;
};

}
