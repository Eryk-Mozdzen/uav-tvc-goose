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
	builder.Connect(plant->GetOutputPort("x"), controller->get_state_input_port());
	builder.Connect(controller->get_control_output_port(), plant->GetInputPort("u"));

	sink->Connect(&builder, plant->GetOutputPort("x"), "x,y,z,phi,theta,psi,beta,dx,dy,dz,dphi,dtheta,dpsi,dbeta");
	sink->Connect(&builder, controller->get_control_output_port(), "ur,a1,a2,a3");

	auto diagram = builder.Build();

	drake::systems::Simulator simulator(*diagram);
	simulator.Initialize();
	//simulator.get_mutable_integrator().set_fixed_step_mode(true);
	//simulator.get_mutable_integrator().set_throw_on_minimum_step_size_violation(false);
	//simulator.get_mutable_integrator().set_maximum_step_size(0.01);
	simulator.AdvanceTo(10);
}
