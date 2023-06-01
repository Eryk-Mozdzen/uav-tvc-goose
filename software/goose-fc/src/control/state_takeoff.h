#pragma once

#include "state_base.h"

class StateTakeOff : public StateBase {
    using StateBase::StateBase;

    void enter() override;
    void execute() override;
    void exit() override;
};
