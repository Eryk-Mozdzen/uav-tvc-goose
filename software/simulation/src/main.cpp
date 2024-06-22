#include <drake/systems/framework/diagram_builder.h>
#include <drake/systems/analysis/simulator.h>
#include <drake/systems/primitives/vector_log_sink.h>
#include <drake/systems/primitives/multiplexer.h>
#include <fstream>

#include "Circle.h"
#include "Lemniscate.h"
#include "Simple3.h"
#include "Plant.h"

int main() {
	drake::systems::DiagramBuilder<double> builder;

	//auto generator = builder.AddSystem<Circle>(0, 0, 2, 6);
	auto generator = builder.AddSystem<Lemniscate>(2, 7);
	auto controller = builder.AddSystem<Simple3>();
	auto plant = builder.AddSystem<Plant>();
	auto mux = builder.AddSystem<drake::systems::Multiplexer>(std::vector{2*6, 4*5, 4*3, 5});
	auto sink = builder.AddSystem<drake::systems::VectorLogSink>(mux->get_output_port().size());

	builder.Connect(generator->get_output_port(), controller->get_trajectory_input_port());
	builder.Connect(generator->get_output_port(), mux->get_input_port(1));
	builder.Connect(controller->get_control_output_port(), plant->get_input_port());
	builder.Connect(controller->get_control_output_port(), mux->get_input_port(3));
	builder.Connect(controller->GetOutputPort("reference"), mux->get_input_port(2));
	builder.Connect(plant->get_output_port(), controller->get_state_input_port());
	builder.Connect(plant->get_output_port(), mux->get_input_port(0));
	builder.Connect(mux->get_output_port(), sink->get_input_port());

	auto diagram = builder.Build();

	drake::systems::Simulator simulator(*diagram);
	simulator.Initialize();
	simulator.AdvanceTo(30);

	auto log = sink->FindLog(simulator.get_context());
	const Eigen::MatrixXd data = log.data().transpose();
	const Eigen::VectorXd time = log.sample_times();

	//Eigen::MatrixXd logs(data.rows(), data.cols() + 1);
    //logs.block(0, 1, data.rows(), data.cols()) = data;
    //logs.col(0) = time;

	const Eigen::MatrixXd logs = time | data;

	std::ofstream file("output.csv");
	file << "t,";
	file << "x,y,z,phi,theta,psi,x1,y1,z1,phi1,theta1,psi1,";
	file << "xd,yd,zd,psid,xd1,yd1,zd1,psid1,xd2,yd2,zd2,psid2,xd3,yd3,zd3,psid3,xd4,yd4,zd4,psid4,";
	file << "phid,thetad,psid,Ftd,phid1,thetad1,psid1,Ftd1,phid2,thetad2,psid2,Ftd2,";
	file << "w,a1,a2,a3,a4";
	file << "\n";
    file << logs.format(Eigen::IOFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, ","));
}
