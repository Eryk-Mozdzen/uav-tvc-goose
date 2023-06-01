#pragma once

#include "state_base.h"

class StateActive : public StateBase {
    using StateBase::StateBase;

    void enter() override;
    void execute() override;
    void exit() override;
};
