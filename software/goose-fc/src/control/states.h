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
    void enter() override;
    void execute() override;
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

    void enter() override;
    void execute() override;

public:
    Landing(events::Negation &event);
};

}
