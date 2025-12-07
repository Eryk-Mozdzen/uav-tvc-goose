#pragma once

#include <drake/systems/framework/leaf_system.h>
#include <drake/systems/framework/diagram.h>

class MPC : public drake::systems::Diagram<double> {
    static constexpr int HP = 20;
    static constexpr int HC = 5;
    static constexpr double T = 0.05;

    static constexpr int NX = 14;
    static constexpr int NT = 12;
    static constexpr int NV = 4;
    static constexpr int NU = 4;

    class PositionController : public drake::systems::LeafSystem<double> {
        void update(const drake::systems::Context<double> &context, drake::systems::DiscreteValues<double> *updates) const;
        void eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const;

    public:
        PositionController();
    };

public:
    MPC();

    const drake::systems::InputPort<double> & get_state_input_port() const;
    const drake::systems::InputPort<double> & get_trajectory_input_port() const;
    const drake::systems::OutputPort<double> & get_control_output_port() const;
};
