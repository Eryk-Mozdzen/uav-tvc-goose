#include <drake/systems/framework/diagram_builder.h>
#include <drake/systems/analysis/simulator.h>

#include "Circle.h"
#include "Lemniscate.h"
#include "Simple3.h"
#include "MPCFL.h"
#include "Plant.h"
#include "Sink.h"

int main() {
	drake::systems::DiagramBuilder<double> builder;

	//auto generator = builder.AddSystem<Circle>(0, 0, 2, 6);
	auto generator = builder.AddSystem<Lemniscate>(2, 20);
	//auto controller = builder.AddSystem<Simple3>();
	auto controller = builder.AddSystem<MPCFL>();
	auto plant = builder.AddSystem<Plant>();
	auto sink = builder.AddSystem<Sink>();

	builder.Connect(generator->get_output_port(), controller->get_trajectory_input_port());
	builder.Connect(controller->get_control_output_port(), plant->get_input_port());
	builder.Connect(plant->get_output_port(), controller->get_state_input_port());

	//sink->Connect(&builder, generator->get_output_port(), "xd,yd,zd,psid,xd1,yd1,zd1,psid1,xd2,yd2,zd2,psid2,xd3,yd3,zd3,psid3,xd4,yd4,zd4,psid4");
	//sink->Connect(&builder, controller->GetOutputPort("reference"), "phid,thetad,psid,Ftd,phid1,thetad1,psid1,Ftd1,phid2,thetad2,psid2,Ftd2");
	sink->Connect(&builder, plant->get_output_port(), "x,y,z,phi,theta,psi,x1,y1,z1,phi1,theta1,psi1");
	sink->Connect(&builder, controller->get_control_output_port(), "w,a1,a2,a3,a4");
	std::stringstream ss;
	ss << "xd,yd,zd,psid,x1d,y1d,z1d,psi1d,";
	for(int i=1; i<20; i++) {
		ss << "xd" << i << ",";
		ss << "yd" << i << ",";
		ss << "zd" << i << ",";
		ss << "psid" << i << ",";
		ss << "x1d" << i << ",";
		ss << "y1d" << i << ",";
		ss << "z1d" << i << ",";
		ss << "psi1d" << i;
		if(i<99) {
			ss << ",";
		}
	}
	sink->Connect(&builder, generator->get_output_port(), ss.str());

	auto diagram = builder.Build();

	drake::systems::Simulator simulator(*diagram);
	simulator.Initialize();
	simulator.AdvanceTo(30);
}
