#pragma once

#include <drake/systems/framework/diagram.h>

class Manual : public drake::systems::Diagram<double> {
public:
    Manual();
};
