#pragma once

#include <drake/systems/framework/diagram.h>

#include "Controller.h"

class Simple3 : public drake::systems::Diagram<double> {
    class OrientationController : public Controller {
        static constexpr double Kp = 650;
        static constexpr double Kd = 51;

        Eigen::VectorX<double> calculate(const Eigen::VectorX<double> &state, const Eigen::VectorX<double> &trajectory) const;

    public:
        OrientationController();
    };

    class PositionController : public Controller {
        static constexpr double Kp = 6;
        static constexpr double Kd = 5;

        Eigen::VectorX<double> calculate(const Eigen::VectorX<double> &state, const Eigen::VectorX<double> &trajectory) const;

    public:
        PositionController();
    };

public:
    Simple3();

    const drake::systems::InputPort<double> & get_state_input_port() const;
    const drake::systems::InputPort<double> & get_trajectory_input_port() const;
    const drake::systems::OutputPort<double> & get_control_output_port() const;
};
