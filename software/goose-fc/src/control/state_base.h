#pragma once

#include "state.h"

class Control;

class StateBase : public sm::State {
protected:
    Control *parent;

public:
    StateBase(Control *parent) : parent{parent} {}
};
