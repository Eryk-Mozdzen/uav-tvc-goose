#pragma once

#include <drake/systems/framework/leaf_system.h>

class Controller : public drake::systems::LeafSystem<double> {
    drake::systems::InputPort<double> *state_port;
    drake::systems::InputPort<double> *trajectory_port;
    drake::systems::OutputPort<double> *control_port;

    void eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const;

    virtual Eigen::VectorX<double> calculate(const Eigen::VectorX<double> &state, const Eigen::VectorX<double> &trajectory) const = 0;

public:
    Controller(const int state_dim, const int trajectory_dim, const int trajectory_derivatives_num, const int control_dim);

    const drake::systems::InputPort<double> & get_state_input_port() const;
    const drake::systems::InputPort<double> & get_trajectory_input_port() const;
    const drake::systems::OutputPort<double> & get_control_output_port() const;
};
