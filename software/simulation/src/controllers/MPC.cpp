#include <drake/systems/framework/diagram_builder.h>
#include <open_optimizer_bindings.hpp>

#include "MPC.h"
#include "Plant.h"
#include "PlantFeedbackLinearization.h"

MPC::PositionController::PositionController() {
    this->DeclareDiscreteState(NV);
    this->DeclarePeriodicDiscreteUpdateEvent(T, 0, &PositionController::update);

    this->DeclareVectorInputPort("x", NX);
    this->DeclareVectorInputPort("x_tr", NT*HP);
    this->DeclareVectorOutputPort("v", NV, &PositionController::eval);
}

void MPC::PositionController::update(const drake::systems::Context<double> &context, drake::systems::DiscreteValues<double> *updates) const {
    const Eigen::Vector<double, NX> x = this->GetInputPort("x").Eval(context);
    const Eigen::Vector<double, NT*HP> x_tr = this->GetInputPort("x_tr").Eval(context);

    double p[OPEN_OPTIMIZER_NUM_PARAMETERS] = {0};
    double v[OPEN_OPTIMIZER_NUM_DECISION_VARIABLES] = {0};

    p[0] = x[0];
    p[1] = x[1];
    p[2] = x[2];
    p[3] = x[3];
    p[4] = x[4];
    p[5] = x[5];
    p[6] = x[6];
    p[7] = x[7];
    p[8] = x[8];
    p[9] = x[9];
    p[10] = x[10];
    p[11] = x[11];
    p[12] = x[12];
    p[13] = x[13];

    for(int i=0; i<HP; i++) {
        p[14 + NT*i + 0] = x_tr[NT*i + 0];
        p[14 + NT*i + 1] = x_tr[NT*i + 1];
        p[14 + NT*i + 2] = x_tr[NT*i + 2];
        p[14 + NT*i + 3] = x_tr[NT*i + 3];
        p[14 + NT*i + 4] = x_tr[NT*i + 4];
        p[14 + NT*i + 5] = x_tr[NT*i + 5];
        p[14 + NT*i + 6] = x_tr[NT*i + 6];
        p[14 + NT*i + 7] = x_tr[NT*i + 7];
    }

    open_optimizerCache *cache = open_optimizer_new();
    open_optimizer_solve(cache, v, p, nullptr, nullptr);
    open_optimizer_free(cache);

    updates->get_mutable_vector().SetAtIndex(0, v[0]);
    updates->get_mutable_vector().SetAtIndex(1, v[1]);
    updates->get_mutable_vector().SetAtIndex(2, v[2]);
    updates->get_mutable_vector().SetAtIndex(3, v[3]);
}

void MPC::PositionController::eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const {
    output->SetFrom(context.get_discrete_state(0));
}

MPC::MPC() {
    drake::systems::DiagramBuilder<double> builder;

    auto position = builder.AddSystem<PositionController>();
    auto orientation = builder.AddSystem<PlantFeedbackLinearization>();

    builder.ExportInput(position->GetInputPort("x_tr"), "x_tr");
    builder.ExportInput(position->GetInputPort("x"), "x");

    builder.ConnectInput("x", orientation->GetInputPort("x"));

    builder.Connect(position->GetOutputPort("v"), orientation->GetInputPort("v"));

    builder.ExportOutput(orientation->GetOutputPort("u"), "u");

    builder.BuildInto(this);
}

const drake::systems::InputPort<double> & MPC::get_state_input_port() const {
    return GetInputPort("x");
}

const drake::systems::InputPort<double> & MPC::get_trajectory_input_port() const {
    return GetInputPort("x_tr");
}

const drake::systems::OutputPort<double> & MPC::get_control_output_port() const {
    return GetOutputPort("u");
}
