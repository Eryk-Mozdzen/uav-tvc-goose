#include <drake/systems/framework/diagram_builder.h>
#include <drake/systems/primitives/discrete_derivative.h>
#include <drake/systems/primitives/multiplexer.h>
#include <drake/systems/primitives/constant_vector_source.h>

#include "Simple3.h"
#include "Params.h"
#include "Plant.h"
#include "Selector.h"
#include "Function.h"

Simple3::OrientationController::OrientationController() : Controller{6, 4, 3, 4} {

}

Eigen::VectorX<double> Simple3::OrientationController::calculate(const Eigen::VectorX<double> &state, const Eigen::VectorX<double> &trajectory) const {
    const Eigen::Vector3d q  = state.segment(3, 3);
    const Eigen::Vector3d q1 = state.segment(9, 3);
    const Eigen::Vector3d y  = trajectory.segment(0, 3);
    const Eigen::Vector3d y1 = trajectory.segment(4, 3);
    const Eigen::Vector3d y2 = trajectory.segment(8, 3);

    const Eigen::Matrix3d M = Plant::M(state.segment(0, 6)).block(3, 3, 3, 3);
    const Eigen::Matrix3d C = Plant::C(state.segment(0, 6), state.segment(6, 6)).block(3, 3, 3, 3);
    const Eigen::Vector3d D = Plant::D(state.segment(0, 6)).segment(3, 3);

    const Eigen::Vector3d v = y2 + Kd*(y1 - q1) + Kp*(y - q);
    const Eigen::Vector3d u = M*v + C*q1 + D;

    return Eigen::Vector4d(trajectory(3), u(0), u(1), u(2));
}

Simple3::PositionController::PositionController() : Controller{6, 4, 5, 4*3} {

}

Eigen::VectorX<double> Simple3::PositionController::calculate(const Eigen::VectorX<double> &state, const Eigen::VectorX<double> &trajectory) const {
    const Eigen::Vector3d q  = state.segment(0, 3);
    const Eigen::Vector3d q1 = state.segment(6, 3);
    const Eigen::Vector3d y  = trajectory.segment(0, 3);
    const Eigen::Vector3d y1 = trajectory.segment(4, 3);
    const Eigen::Vector3d y2 = trajectory.segment(8, 3);
    const Eigen::Vector3d y3 = trajectory.segment(12, 3);
    const Eigen::Vector3d y4 = trajectory.segment(16, 3);

    const double psi = trajectory(3);
    const double psi1 = trajectory(7);
    const double psi2 = trajectory(11);

    const Eigen::Matrix3d R {
        { std::cos(psi), std::sin(psi), 0},
        {-std::sin(psi), std::cos(psi), 0},
        {0, 0, 1}
    };

    const Eigen::Matrix3d Rp {
        {-std::sin(psi),  std::cos(psi), 0},
        {-std::cos(psi), -std::sin(psi), 0},
        {0, 0, 0}
    };

    const Eigen::Matrix3d Rpp {
        {-std::cos(psi), -std::sin(psi), 0},
        { std::sin(psi), -std::cos(psi), 0},
        {0, 0, 0}
    };

    const Eigen::Vector3d e  = y  - q;
    const Eigen::Vector3d e1 = y1 - q1;
    const Eigen::Vector3d e2 = y2 - (y2 + Kd*e1 + Kp*e);
    const Eigen::Vector3d e3 = y3 - (y3 + Kd*e2 + Kp*e1);

    const Eigen::Vector3d v  = R*(y2 + Kd*e1 + Kp*e);
    const Eigen::Vector3d v1 = R*(y3 + Kd*e2 + Kp*e1) + Rp*psi1*(y2 + Kd*e1 + Kp*e);
    const Eigen::Vector3d v2 = R*(y4 + Kd*e3 + Kp*e2) + 2*Rp*psi1*(y3 + Kd*e2 + Kp*e1) + Rp*psi2*(y2 + Kd*e1 + Kp*e) + Rpp*pow(psi1, 2)*(y2 + Kd*e1 + Kp*e);

    Eigen::Vector<double, 12> output;

    output.segment(0, 4) = Eigen::Vector<double, 4>{
        atan2(-v(1), Params::g + v(2)),
        atan2(v(0)*(Params::g + v(2))/sqrt(pow(v(1), 2) + pow(Params::g + v(2), 2)), Params::g + v(2)),
        trajectory(3),
        Params::m*sqrt(pow(Params::g + v(2), 2)*(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2))/(pow(v(1), 2) + pow(Params::g + v(2), 2)))*sqrt(pow(v(1), 2) + pow(Params::g + v(2), 2))/(Params::g + v(2))
    };

    output.segment(4, 4) = Eigen::Vector<double, 4>{
        (v(1)*v1(2) - v1(1)*(Params::g + v(2)))/(pow(v(1), 2) + pow(Params::g + v(2), 2)),
        (-v(0)*v1(2)*(pow(v(1), 2) + pow(Params::g + v(2), 2)) - v(0)*(Params::g + v(2))*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) + (pow(v(1), 2) + pow(Params::g + v(2), 2))*(v(0)*v1(2) + v1(0)*(Params::g + v(2))))/((Params::g + v(2))*sqrt(pow(v(1), 2) + pow(Params::g + v(2), 2))*(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2))),
        trajectory(7),
        0
    };

    output.segment(8, 4) = Eigen::Vector<double, 4>{
        (-2*v(1)*v1(2)*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) + 2*v1(1)*(Params::g + v(2))*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) - (pow(v(1), 2) + pow(Params::g + v(2), 2))*(-v(1)*v2(2) + v2(1)*(Params::g + v(2))))/pow(pow(v(1), 2) + pow(Params::g + v(2), 2), 2),
        (-v(0)*pow(v1(2), 2)*pow(pow(v(1), 2) + pow(Params::g + v(2), 2), 2)*(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2)) + 2*v(0)*v1(2)*(pow(v(1), 2) + pow(Params::g + v(2), 2))*(-pow(v(0), 2)*(Params::g + v(2))*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) + v(0)*(pow(v(1), 2) + pow(Params::g + v(2), 2))*(v(0)*v1(2) + v1(0)*(Params::g + v(2))) + v1(2)*pow(pow(v(1), 2) + pow(Params::g + v(2), 2), 2)) + 3*v(0)*pow(Params::g + v(2), 2)*pow(v(1)*v1(1) + v1(2)*(Params::g + v(2)), 2)*(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2)) + v1(2)*(pow(v(1), 2) + pow(Params::g + v(2), 2))*(-v(0)*(Params::g + v(2))*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) + (pow(v(1), 2) + pow(Params::g + v(2), 2))*(v(0)*v1(2) + v1(0)*(Params::g + v(2))))*(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2)) + (Params::g + v(2))*pow(pow(v(1), 2) + pow(Params::g + v(2), 2), 2)*(v1(0)*v1(2) + v2(0)*(Params::g + v(2)))*(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2)) - (Params::g + v(2))*(pow(v(1), 2) + pow(Params::g + v(2), 2))*(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2))*(v(0)*v1(2)*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) + v(0)*(Params::g + v(2))*(v(1)*v2(1) + pow(v1(1), 2) + pow(v1(2), 2) + v2(2)*(Params::g + v(2))) + 2*v1(0)*(Params::g + v(2))*(v(1)*v1(1) + v1(2)*(Params::g + v(2)))) - 2*(-v(0)*(Params::g + v(2))*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) + (pow(v(1), 2) + pow(Params::g + v(2), 2))*(v(0)*v1(2) + v1(0)*(Params::g + v(2))))*(-pow(v(0), 2)*(Params::g + v(2))*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) + v(0)*(pow(v(1), 2) + pow(Params::g + v(2), 2))*(v(0)*v1(2) + v1(0)*(Params::g + v(2))) + v1(2)*pow(pow(v(1), 2) + pow(Params::g + v(2), 2), 2)))/(pow(Params::g + v(2), 2)*pow(pow(v(1), 2) + pow(Params::g + v(2), 2), 3.0/2.0)*pow(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2), 2)),
        trajectory(11),
        0
    };

    assert(!output.hasNaN());

    return output;
}

Simple3::Simple3() {
    drake::systems::DiagramBuilder<double> builder;

    auto position = builder.AddSystem<PositionController>();
    auto orientation = builder.AddSystem<OrientationController>();
    auto conv = builder.AddSystem<Function>(4, 5, [](const Eigen::VectorX<double> &generalized) {return Plant::decodeControls(generalized);});

    builder.ExportInput(position->get_trajectory_input_port(), "trajectory");
    builder.ExportInput(position->get_state_input_port(), "state");

    builder.ConnectInput("state", orientation->get_state_input_port());

    builder.Connect(position->get_control_output_port(), orientation->get_trajectory_input_port());
    builder.Connect(orientation->get_control_output_port(), conv->get_input_port());

    builder.ExportOutput(conv->get_output_port(), "control");
    builder.ExportOutput(position->get_control_output_port(), "reference");

    builder.BuildInto(this);
}

const drake::systems::InputPort<double> & Simple3::get_state_input_port() const {
    return GetInputPort("state");
}

const drake::systems::InputPort<double> & Simple3::get_trajectory_input_port() const {
    return GetInputPort("trajectory");
}

const drake::systems::OutputPort<double> & Simple3::get_control_output_port() const {
    return GetOutputPort("control");
}
