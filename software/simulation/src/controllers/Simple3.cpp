#include <drake/systems/framework/diagram_builder.h>

#include "Simple3.h"
#include "Params.h"
#include "Plant.h"

Simple3::PositionController::PositionController() {
    this->DeclareVectorInputPort("trajectory", 4*5);
    this->DeclareVectorInputPort("state", 2*6);
    this->DeclareVectorOutputPort("reference", 4*3, &PositionController::eval);
}

void Simple3::PositionController::eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const {
    const Eigen::VectorXd state = this->GetInputPort("state").Eval(context);
    const Eigen::VectorXd trajectory = this->GetInputPort("trajectory").Eval(context);

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
    //const double psi3 = trajectory(15);

    const Eigen::Matrix3d R {
        {std::cos(-psi), -std::sin(-psi), 0},
        {std::sin(-psi),  std::cos(-psi), 0},
        {0, 0, 1}
    };

    const Eigen::Matrix3d Rp {
        {-std::sin(-psi)*-1, -std::cos(-psi)*-1, 0},
        { std::cos(-psi)*-1, -std::sin(-psi)*-1, 0},
        {0, 0, 0}
    };

    const Eigen::Matrix3d Rpp {
        {-std::cos(-psi)*-1*-1,  std::sin(-psi)*-1*-1, 0},
        {-std::sin(-psi)*-1*-1, -std::cos(-psi)*-1*-1, 0},
        {0, 0, 0}
    };

    /*const Eigen::Matrix3d Rppp {
        { std::sin(-psi)*-1*-1*-1, std::cos(-psi)*-1*-1*-1, 0},
        {-std::cos(-psi)*-1*-1*-1, std::sin(-psi)*-1*-1*-1, 0},
        {0, 0, 0}
    };*/

    const Eigen::Vector3d e  = y  - q;
    const Eigen::Vector3d e1 = y1 - q1;
    const Eigen::Vector3d e2 = y2 - (y2 + Kd*e1 + Kp*e);
    const Eigen::Vector3d e3 = y3 - (y3 + Kd*e2 + Kp*e1);

    const Eigen::Vector3d v  = R*(y2 + Kd*e1 + Kp*e);
    const Eigen::Vector3d v1 = R*(y3 + Kd*e2 + Kp*e1) + Rp*psi1*(y2 + Kd*e1 + Kp*e);
    const Eigen::Vector3d v2 = R*(y4 + Kd*e3 + Kp*e2) + 2*Rp*psi1*(y3 + Kd*e2 + Kp*e1) + Rp*psi2*(y2 + Kd*e1 + Kp*e) + Rpp*psi1*psi1*(y2 + Kd*e1 + Kp*e);

    /*const Eigen::Vector3d e  = R*(y - q);
    const Eigen::Vector3d e1 = R*(y1 - q1) + Rp*psi1*(y - q);
    const Eigen::Vector3d v  = R*y2 + Kd*e1 + Kp*e;

    const Eigen::Vector3d q2 = R.inverse()*v;
    const Eigen::Vector3d e2 = R*(y2 - q2) + Rp*psi1*(y1 - q1) + Rp*psi1*(y1 - q1) + Rp*psi2*(y - q) + Rpp*psi1*psi1*(y - q);
    const Eigen::Vector3d v1 = R*y3 + Kd*e2 + Kp*e1 + Rp*psi1*y2;

    const Eigen::Vector3d q3 = R.inverse()*v1 - R.inverse()*R.inverse()*Rp*psi1*v;
    const Eigen::Vector3d e3 = R*(y3 - q3) + Rp*psi1*(y2 - q2)
    + Rp*psi1*(y2 - q2) + Rp*psi2*(y1 - q1) + Rpp*psi1*psi1*(y1 - q1)
    + Rp*psi1*(y2 - q2) + Rp*psi2*(y1 - q1) + Rpp*psi1*psi1*(y1 - q1)
    + Rp*psi2*(y1 - q1) + Rp*psi3*(y - q) + Rpp*psi1*psi2*(y - q)
    + Rpp*psi1*psi1*(y1 - q1) + Rpp*psi1*psi2*(y - q) + Rpp*psi2*psi1*(y - q) + Rppp*psi1*psi1*psi1*(y - q);
    const Eigen::Vector3d v2 = R*y4 + Kd*e3 + Kp*e2 + Rp*psi1*y3 + Rp*psi1*y3 + Rp*psi2*y2 + Rpp*psi1*psi1*y2;*/

    Eigen::Vector<double, 12> out;

    out.segment(0, 4) = Eigen::Vector<double, 4>{
        atan2(-v(1), Params::g + v(2)),
        atan2(v(0)*(Params::g + v(2))/sqrt(pow(v(1), 2) + pow(Params::g + v(2), 2)), Params::g + v(2)),
        trajectory(3),
        Params::m*sqrt(pow(Params::g + v(2), 2)*(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2))/(pow(v(1), 2) + pow(Params::g + v(2), 2)))*sqrt(pow(v(1), 2) + pow(Params::g + v(2), 2))/(Params::g + v(2))
    };

    out.segment(4, 4) = Eigen::Vector<double, 4>{
        (v(1)*v1(2) - v1(1)*(Params::g + v(2)))/(pow(v(1), 2) + pow(Params::g + v(2), 2)),
        (-v(0)*v1(2)*(pow(v(1), 2) + pow(Params::g + v(2), 2)) - v(0)*(Params::g + v(2))*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) + (pow(v(1), 2) + pow(Params::g + v(2), 2))*(v(0)*v1(2) + v1(0)*(Params::g + v(2))))/((Params::g + v(2))*sqrt(pow(v(1), 2) + pow(Params::g + v(2), 2))*(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2))),
        trajectory(7),
        0
    };

    out.segment(8, 4) = Eigen::Vector<double, 4>{
        (-2*v(1)*v1(2)*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) + 2*v1(1)*(Params::g + v(2))*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) - (pow(v(1), 2) + pow(Params::g + v(2), 2))*(-v(1)*v2(2) + v2(1)*(Params::g + v(2))))/pow(pow(v(1), 2) + pow(Params::g + v(2), 2), 2),
        (-v(0)*pow(v1(2), 2)*pow(pow(v(1), 2) + pow(Params::g + v(2), 2), 2)*(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2)) + 2*v(0)*v1(2)*(pow(v(1), 2) + pow(Params::g + v(2), 2))*(-pow(v(0), 2)*(Params::g + v(2))*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) + v(0)*(pow(v(1), 2) + pow(Params::g + v(2), 2))*(v(0)*v1(2) + v1(0)*(Params::g + v(2))) + v1(2)*pow(pow(v(1), 2) + pow(Params::g + v(2), 2), 2)) + 3*v(0)*pow(Params::g + v(2), 2)*pow(v(1)*v1(1) + v1(2)*(Params::g + v(2)), 2)*(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2)) + v1(2)*(pow(v(1), 2) + pow(Params::g + v(2), 2))*(-v(0)*(Params::g + v(2))*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) + (pow(v(1), 2) + pow(Params::g + v(2), 2))*(v(0)*v1(2) + v1(0)*(Params::g + v(2))))*(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2)) + (Params::g + v(2))*pow(pow(v(1), 2) + pow(Params::g + v(2), 2), 2)*(v1(0)*v1(2) + v2(0)*(Params::g + v(2)))*(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2)) - (Params::g + v(2))*(pow(v(1), 2) + pow(Params::g + v(2), 2))*(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2))*(v(0)*v1(2)*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) + v(0)*(Params::g + v(2))*(v(1)*v2(1) + pow(v1(1), 2) + pow(v1(2), 2) + v2(2)*(Params::g + v(2))) + 2*v1(0)*(Params::g + v(2))*(v(1)*v1(1) + v1(2)*(Params::g + v(2)))) - 2*(-v(0)*(Params::g + v(2))*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) + (pow(v(1), 2) + pow(Params::g + v(2), 2))*(v(0)*v1(2) + v1(0)*(Params::g + v(2))))*(-pow(v(0), 2)*(Params::g + v(2))*(v(1)*v1(1) + v1(2)*(Params::g + v(2))) + v(0)*(pow(v(1), 2) + pow(Params::g + v(2), 2))*(v(0)*v1(2) + v1(0)*(Params::g + v(2))) + v1(2)*pow(pow(v(1), 2) + pow(Params::g + v(2), 2), 2)))/(pow(Params::g + v(2), 2)*pow(pow(v(1), 2) + pow(Params::g + v(2), 2), 3.0/2.0)*pow(pow(v(0), 2) + pow(v(1), 2) + pow(Params::g + v(2), 2), 2)),
        trajectory(11),
        0
    };

    assert(!out.hasNaN());

    output->SetFromVector(out);
}

Simple3::OrientationController::OrientationController() {
    this->DeclareVectorInputPort("trajectory", 4*3);
    this->DeclareVectorInputPort("state", 2*6);
    this->DeclareVectorOutputPort("control", 5, &OrientationController::eval);
}

void Simple3::OrientationController::eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const {
    const Eigen::VectorXd state = this->GetInputPort("state").Eval(context);
    const Eigen::VectorXd trajectory = this->GetInputPort("trajectory").Eval(context);

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

    output->SetFromVector(Plant::decodeControls(Eigen::Vector4d(trajectory(3), u(0), u(1), u(2))));
}

Simple3::Simple3() {
    drake::systems::DiagramBuilder<double> builder;

    auto position = builder.AddSystem<PositionController>();
    auto orientation = builder.AddSystem<OrientationController>();

    builder.ExportInput(position->GetInputPort("trajectory"), "trajectory");
    builder.ExportInput(position->GetInputPort("state"), "state");

    builder.ConnectInput("state", orientation->GetInputPort("state"));

    builder.Connect(position->GetOutputPort("reference"), orientation->GetInputPort("trajectory"));

    builder.ExportOutput(orientation->GetOutputPort("control"), "control");
    builder.ExportOutput(position->GetOutputPort("reference"), "reference");

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
