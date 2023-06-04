#pragma once

#include "state.h"
#include "comm.h"

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
    void enter() override;
    void execute() override;
};

class Landing : public sm::State {
    void enter() override;
    void execute() override;
};

}
