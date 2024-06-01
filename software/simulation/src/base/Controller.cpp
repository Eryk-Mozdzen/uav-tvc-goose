#include "Controller.h"

Controller::Controller(const int state_dim, const int trajectory_dim, const int trajectory_derivatives_num, const int control_dim) {
    trajectory_port = &DeclareVectorInputPort("trajectory", trajectory_derivatives_num*trajectory_dim);
    state_port = &DeclareVectorInputPort("state", 2*state_dim);

    control_port = &DeclareVectorOutputPort("control", control_dim, &Controller::eval);
}

void Controller::eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const {
    const Eigen::VectorX<double> trajectory = trajectory_port->Eval(context);
    const Eigen::VectorX<double> state = state_port->Eval(context);

    const Eigen::VectorX<double> u = calculate(state, trajectory);

    output->SetFromVector(u);
}

const drake::systems::InputPort<double> & Controller::get_state_input_port() const {
    return *state_port;
}

const drake::systems::InputPort<double> & Controller::get_trajectory_input_port() const {
    return *trajectory_port;
}

const drake::systems::OutputPort<double> & Controller::get_control_output_port() const {
    return *control_port;
}
