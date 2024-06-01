#include <drake/systems/framework/diagram_builder.h>
#include <drake/systems/primitives/integrator.h>
#include <drake/systems/primitives/constant_vector_source.h>
#include <drake/systems/primitives/multiplexer.h>

#include "Manual.h"
#include "Gamepad.h"

Manual::Manual() {

    drake::systems::DiagramBuilder<double> builder;

    auto gamepad = builder.AddSystem<Gamepad>();
    auto integrator = builder.AddSystem<drake::systems::Integrator>(4);
    auto zero = builder.AddSystem<drake::systems::ConstantVectorSource>(Eigen::Vector4d(0, 0, 0, 0));
    auto mux = builder.AddSystem<drake::systems::Multiplexer>(std::vector<int>{4, 4, 4});

    builder.Connect(gamepad->get_output_port(), integrator->get_input_port());
    builder.Connect(integrator->get_output_port(), mux->get_input_port(0));
    builder.Connect(gamepad->get_output_port(), mux->get_input_port(1));
    builder.Connect(zero->get_output_port(), mux->get_input_port(2));

    builder.ExportOutput(mux->get_output_port(), "trajectory");

    builder.BuildInto(this);
}
