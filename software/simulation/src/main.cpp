#include <drake/systems/framework/diagram_builder.h>
#include <drake/systems/analysis/simulator.h>

//#include "Circle.h"
//#include "Lemniscate.h"
//#include "Simple3.h"
//#include "MPCFL.h"
#include "Plant.h"
#include "PlantFeedbackLinearization.h"
#include "Sink.h"

class Controller : public drake::systems::LeafSystem<double> {
    void eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const {
		const Eigen::Vector<double, 14> x = this->GetInputPort("x").Eval(context);

		const double t = context.get_time();

		const double y0 = +0.5*sin(2*3.1415*t);
		const double y1 = +0.5*cos(2*3.1415*t)*2*3.1415;
		const double y2 = -0.5*sin(2*3.1415*t)*2*3.1415*2*3.1415;

		const double k0 = 2;
		const double k1 = 3;

		output->SetAtIndex(0, y2 - k1*(x[10] - y1) - k0*(x[3] - y0));
		output->SetAtIndex(1, y2 - k1*(x[11] - y1) - k0*(x[4] - y0));
		output->SetAtIndex(2, y2 - k1*(x[12] - y1) - k0*(x[5] - y0));
		output->SetAtIndex(3,                  y1  - k0*(Plant::K_f*x[13]*x[13] - (y0 + 1)));
	}

public:
    Controller() {
		this->DeclareVectorInputPort("x", 14);
		this->DeclareVectorOutputPort("v", 4, &Controller::eval);
	}
};

int main() {
	drake::systems::DiagramBuilder<double> builder;

	auto control = builder.AddSystem<Controller>();
	auto linear = builder.AddSystem<PlantFeedbackLinearization>();
	auto plant = builder.AddSystem<Plant>();
	auto sink = builder.AddSystem<Sink>();

	builder.Connect(control->GetOutputPort("v"), linear->GetInputPort("v"));
	builder.Connect(linear->GetOutputPort("u"), plant->GetInputPort("u"));
	builder.Connect(plant->GetOutputPort("x"), linear->GetInputPort("x"));
	builder.Connect(plant->GetOutputPort("x"), control->GetInputPort("x"));

	sink->Connect(&builder, plant->GetOutputPort("x"), "x,y,z,phi,theta,psi,beta,dx,dy,dz,dphi,dtheta,dpsi,dbeta");

	auto diagram = builder.Build();

	drake::systems::Simulator simulator(*diagram);
	simulator.Initialize();
	//simulator.get_mutable_integrator().set_fixed_step_mode(true);
	//simulator.get_mutable_integrator().set_throw_on_minimum_step_size_violation(false);
	//simulator.get_mutable_integrator().set_maximum_step_size(0.01);
	simulator.AdvanceTo(30);
}
