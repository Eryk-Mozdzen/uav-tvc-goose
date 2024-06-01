#include <fstream>
#include <QApplication>
#include <drake/systems/framework/diagram_builder.h>

#include "Simulator.h"
#include "Square.h"
#include "Circle.h"
#include "Lemniscate.h"
#include "Manual.h"
#include "Simple2.h"
#include "Plant.h"
#include "VisualClient.h"
#include "GraphXY.h"
#include "Chart.h"

int main(int argc, char **argv) {
	QApplication app(argc, argv);

	drake::systems::DiagramBuilder<double> builder;

	//auto generator = builder.AddSystem<Square>(4, 0.5, 20);
	//auto generator = builder.AddSystem<Circle>(0, 0, 2, 5);
	auto generator = builder.AddSystem<Lemniscate>(2, 10);
	//auto generator = builder.AddSystem<Manual>();
	auto controller = builder.AddSystem<Simple2>();
	auto plant = builder.AddSystem<Plant>();
	auto client = builder.AddSystem<VisualClient>();
	auto trajectory_xy = builder.AddSystem<GraphXY>("trajectory XY", "%+2.0f", 4);
	auto actuators_rotor = builder.AddSystem<Chart>("rotor", "velocity [rad/s]", "%4.0f", 0, 2000);
	auto actuators_vanes = builder.AddSystem<Chart>("vanes", "angle [deg]", "%+3.0f", -10, 10);

	trajectory_xy->AddSeries("desired", Eigen::Matrix<double, 2, 12>({
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	}), Qt::black, Qt::DashLine, 1);
	trajectory_xy->AddSeries("current", Eigen::Matrix<double, 2, 12>({
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	}), Qt::red, Qt::SolidLine, 2);
	actuators_rotor->AddSeries("rotor", Eigen::Vector<double, 5>({1, 0, 0, 0, 0}), Qt::black, Qt::SolidLine, 2);
	actuators_vanes->AddSeries("vanes", Eigen::Matrix<double, 4, 5>({
		{0, 1, 0, 0, 0},
		{0, 0, 1, 0, 0},
		{0, 0, 0, 1, 0},
		{0, 0, 0, 0, 1}
	})*57.2957);

	builder.Connect(generator->get_output_port(), controller->get_trajectory_input_port());
	builder.Connect(generator->get_output_port(), trajectory_xy->GetInputPort("desired"));
	builder.Connect(controller->get_control_output_port(), plant->get_input_port());
	builder.Connect(controller->get_control_output_port(), actuators_rotor->GetInputPort("rotor"));
	builder.Connect(controller->get_control_output_port(), actuators_vanes->GetInputPort("vanes"));
	builder.Connect(plant->get_output_port(), controller->get_state_input_port());
	builder.Connect(plant->get_output_port(), trajectory_xy->GetInputPort("current"));
	builder.Connect(plant->get_output_port(), client->get_input_port());

	auto diagram = builder.Build();

	std::ofstream file("diagram.dot");
    file << diagram->GetGraphvizString();
    file.close();

	Simulator<double> simulator(*diagram);
	simulator.set_target_realtime_rate(1);
	simulator.Initialize();
	simulator.StartAdvance();

	return app.exec();
}
