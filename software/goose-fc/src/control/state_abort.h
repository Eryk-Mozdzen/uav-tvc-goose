#pragma once

#include "state_base.h"

class StateAbort : public StateBase {
    using StateBase::StateBase;

    void enter() override;
    void execute() override;
    void exit() override;
};