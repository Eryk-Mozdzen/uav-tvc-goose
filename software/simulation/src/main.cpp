#include <drake/systems/framework/diagram_builder.h>
#include <drake/systems/analysis/simulator.h>

#include "Lemniscate.h"
#include "MPC.h"
#include "Plant.h"
#include "Sink.h"

int main() {
	drake::systems::DiagramBuilder<double> builder;

	auto generator = builder.AddSystem<Lemniscate>(2, 10);
	auto controller = builder.AddSystem<MPC>();
	auto plant = builder.AddSystem<Plant>();
	auto sink = builder.AddSystem<Sink>();

	builder.Connect(generator->get_output_port(), controller->get_trajectory_input_port());
	builder.Connect(plant->get_state_output_port(), controller->get_state_input_port());
	builder.Connect(controller->get_control_output_port(), plant->get_control_input_port());

	sink->Connect(&builder, plant->get_state_output_port(), "x,y,z,phi,theta,psi,beta,dx,dy,dz,dphi,dtheta,dpsi,dbeta");
	sink->Connect(&builder, controller->get_control_output_port(), "ur,a1,a2,a3");

	auto diagram = builder.Build();

	drake::systems::Simulator simulator(*diagram);
	simulator.Initialize();
	simulator.AdvanceTo(30);
}
