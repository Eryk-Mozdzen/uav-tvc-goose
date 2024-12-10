#include <drake/systems/framework/diagram_builder.h>
#include <open_optimizer_bindings.hpp>

#include "MPCFL.h"
#include "Plant.h"

MPCFL::MPC::MPC() {
    this->DeclareDiscreteState(NV);
    this->DeclarePeriodicDiscreteUpdateEvent(T, 0, &MPC::update);

    this->DeclareVectorInputPort("x", NX);
    this->DeclareVectorInputPort("x_tr", NT*HP);
    this->DeclareVectorOutputPort("v", NV, &MPC::eval);
}

void MPCFL::MPC::update(const drake::systems::Context<double> &context, drake::systems::DiscreteValues<double> *updates) const {
    const Eigen::Vector<double, NX> state = this->GetInputPort("x").Eval(context);
    const Eigen::Vector<double, NT*HP> trajectory = this->GetInputPort("x_tr").Eval(context);

    double p[OPEN_OPTIMIZER_NUM_PARAMETERS] = {0};
    double v[OPEN_OPTIMIZER_NUM_DECISION_VARIABLES] = {0};

    for(int i=0; i<NX; i++) {
        p[i] = state[i];
    }

    for(int i=0; i<NT*HP; i++) {
        p[NX + i] = trajectory[i];
    }

    open_optimizerCache *cache = open_optimizer_new();
    open_optimizer_solve(cache, v, p, nullptr, nullptr);
    open_optimizer_free(cache);

    updates->get_mutable_vector().SetAtIndex(0, v[0]);
    updates->get_mutable_vector().SetAtIndex(1, v[1]);
    updates->get_mutable_vector().SetAtIndex(2, v[2]);
    updates->get_mutable_vector().SetAtIndex(3, v[3]);
}

void MPCFL::MPC::eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const {
    output->SetFrom(context.get_discrete_state(0));
}

MPCFL::FeedbackLinearization::FeedbackLinearization() {
    this->DeclareVectorInputPort("v", NV);
    this->DeclareVectorInputPort("x", NX);
    this->DeclareVectorOutputPort("u", NU, &FeedbackLinearization::eval);
}

void MPCFL::FeedbackLinearization::eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const {
    (void)context;
    (void)output;
    /*const Eigen::VectorXd v = this->GetInputPort("v").Eval(context);
    const Eigen::VectorXd x = this->GetInputPort("x").Eval(context);
    const Eigen::VectorXd x_tr = this->GetInputPort("x_tr").Eval(context);

    const Eigen::Matrix3d M = Plant::M(state.segment(0, 6)).block(3, 3, 3, 3);
    const Eigen::Matrix3d C = Plant::C(state.segment(0, 6), state.segment(6, 6)).block(3, 3, 3, 3);
    const Eigen::Vector3d D = Plant::D(state.segment(0, 6)).segment(3, 3);

    const Eigen::Vector3d u = M*v + C*q1 + D;

    output->SetFromVector(Plant::decodeControls(Eigen::Vector4d(u(0), u(1), u(2))));*/
}

MPCFL::MPCFL() {
    drake::systems::DiagramBuilder<double> builder;

    auto position = builder.AddSystem<MPC>();
    auto orientation = builder.AddSystem<FeedbackLinearization>();

    builder.ExportInput(position->GetInputPort("x_tr"), "x_tr");
    builder.ExportInput(position->GetInputPort("x"), "x");

    builder.ConnectInput("x", orientation->GetInputPort("x"));

    builder.ExportOutput(orientation->GetOutputPort("u"), "u");

    builder.BuildInto(this);
}

const drake::systems::InputPort<double> & MPCFL::get_state_input_port() const {
    return GetInputPort("x");
}

const drake::systems::InputPort<double> & MPCFL::get_trajectory_input_port() const {
    return GetInputPort("x_tr");
}

const drake::systems::OutputPort<double> & MPCFL::get_control_output_port() const {
    return GetOutputPort("u");
}
