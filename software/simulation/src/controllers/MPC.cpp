#include <iostream>

#include "MPC.h"
#include "open_optimizer_bindings.hpp"

MPC::MPC() {
    this->DeclareVectorInputPort("state", 12);
    this->DeclareVectorInputPort("trajectory", 4*100);
    this->DeclareVectorOutputPort("control", 5, &MPC::eval);
}

void MPC::eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const {
    const Eigen::Vector<double, 12> state = this->GetInputPort("state").Eval(context);
    const Eigen::Vector<double, 400> trajectory = this->GetInputPort("trajectory").Eval(context);

    double p[OPEN_OPTIMIZER_NUM_PARAMETERS];
    double u[OPEN_OPTIMIZER_NUM_DECISION_VARIABLES] = {0};

    for(int i=0; i<state.size(); i++) {
        p[i] = state[i];
    }

    for(int i=0; i<trajectory.size(); i++) {
        p[state.size() + i] = trajectory[i];
    }

    open_optimizerCache *cache = open_optimizer_new();
    const open_optimizerSolverStatus status = open_optimizer_solve(cache, u, p, nullptr, nullptr);
    open_optimizer_free(cache);

    if(status.exit_status!=open_optimizerExitStatus::open_optimizerConverged) {
        std::cout << static_cast<int>(status.exit_status) << std::endl;
    }

    std::cout << "Exit Status: " << static_cast<int>(status.exit_status) << std::endl;
    std::cout << "Number of Outer Iterations: " << status.num_outer_iterations << std::endl;
    std::cout << "Number of Inner Iterations: " << status.num_inner_iterations << std::endl;
    std::cout << "Last Problem Norm FPR: " << status.last_problem_norm_fpr << std::endl;
    std::cout << "Total Solve Time (ns): " << status.solve_time_ns << std::endl;
    std::cout << "Penalty Value: " << status.penalty << std::endl;
    std::cout << "Delta Y Norm Over C: " << status.delta_y_norm_over_c << std::endl;
    std::cout << "F2 Norm: " << status.f2_norm << std::endl;
    std::cout << "Cost: " << status.cost << std::endl;
    std::cout << u[0] << " " << u[1] << " " << u[2] << " " << u[3] << " " << u[4] << std::endl;
    std::cout << std::endl;

    output->SetFromVector(Eigen::Vector<double, 5>(u[0], u[1], u[2], u[3], u[4]));
}

const drake::systems::InputPort<double> & MPC::get_state_input_port() const {
    return GetInputPort("state");
}

const drake::systems::InputPort<double> & MPC::get_trajectory_input_port() const {
    return GetInputPort("trajectory");
}

const drake::systems::OutputPort<double> & MPC::get_control_output_port() const {
    return GetOutputPort("control");
}
