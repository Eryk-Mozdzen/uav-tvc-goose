#pragma once

#include <drake/systems/framework/leaf_system.h>
#include <drake/systems/framework/diagram.h>

class Simple3 : public drake::systems::Diagram<double> {
    class PositionController : public drake::systems::LeafSystem<double> {
        static constexpr double Kp = 6;
        static constexpr double Kd = 5;

        void eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const;

    public:
        PositionController();
    };

    class OrientationController : public drake::systems::LeafSystem<double> {
        static constexpr double Kp = 650;
        static constexpr double Kd = 51;

        void eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const;

    public:
        OrientationController();
    };

public:
    Simple3();

    const drake::systems::InputPort<double> & get_state_input_port() const;
    const drake::systems::InputPort<double> & get_trajectory_input_port() const;
    const drake::systems::OutputPort<double> & get_control_output_port() const;
};
