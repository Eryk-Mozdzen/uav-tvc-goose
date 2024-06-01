#pragma once

#include <drake/systems/framework/diagram.h>

#include "Controller.h"

class Simple2 : public drake::systems::Diagram<double> {

    class HoverController : public Controller {
        const Eigen::Matrix<double, 4, 4> Kp = 5*Eigen::Matrix<double, 4, 4>::Identity();
        const Eigen::Matrix<double, 4, 4> Kd = 10*Eigen::Matrix<double, 4, 4>::Identity();

        Eigen::VectorX<double> calculate(const Eigen::VectorX<double> &state, const Eigen::VectorX<double> &trajectory) const;

    public:
        HoverController();
    };

    class PositionController : public Controller {
        const Eigen::Matrix<double, 2, 2> Kp = 3*Eigen::Matrix<double, 2, 2>::Identity();
        const Eigen::Matrix<double, 2, 2> Kd = 3*Eigen::Matrix<double, 2, 2>::Identity();

        Eigen::VectorX<double> calculate(const Eigen::VectorX<double> &state, const Eigen::VectorX<double> &trajectory) const;

    public:
        PositionController();
    };

public:
    Simple2();

    const drake::systems::InputPort<double> & get_state_input_port() const;
    const drake::systems::InputPort<double> & get_trajectory_input_port() const;
    const drake::systems::OutputPort<double> & get_control_output_port() const;
};
