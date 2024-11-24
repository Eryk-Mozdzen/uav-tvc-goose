#pragma once

#include <drake/systems/framework/leaf_system.h>

class MPC : public drake::systems::LeafSystem<double> {
    void eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const;

public:
    MPC();

    const drake::systems::InputPort<double> & get_state_input_port() const;
    const drake::systems::InputPort<double> & get_trajectory_input_port() const;
    const drake::systems::OutputPort<double> & get_control_output_port() const;
};
