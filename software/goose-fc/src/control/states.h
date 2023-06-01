#pragma once

#include "state.h"

namespace states {

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
