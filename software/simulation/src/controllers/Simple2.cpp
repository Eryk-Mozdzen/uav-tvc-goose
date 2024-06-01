#include <drake/systems/framework/diagram_builder.h>
#include <drake/systems/primitives/discrete_derivative.h>
#include <drake/systems/primitives/multiplexer.h>
#include <drake/systems/primitives/constant_vector_source.h>

#include "Simple2.h"
#include "Params.h"
#include "Plant.h"
#include "Selector.h"
#include "Function.h"

Simple2::HoverController::HoverController() : Controller{6, 4, 3, 4} {

}

Eigen::VectorX<double> Simple2::HoverController::calculate(const Eigen::VectorX<double> &state, const Eigen::VectorX<double> &trajectory) const {
    const Eigen::Vector<double, 6> q = state.segment(0, 6);
    const Eigen::Vector<double, 6> dq = state.segment(6, 6);
    const Eigen::Vector<double, 4> y = trajectory.segment(0, 4);
    const Eigen::Vector<double, 4> dy = trajectory.segment(4, 4);
    const Eigen::Vector<double, 4> ddy = trajectory.segment(8, 4);

    const Eigen::Matrix<double, 4, 6> H {
        {0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 1}
    };

    const Eigen::Vector<double, 4> e = y - H*q;
    const Eigen::Vector<double, 4> de = dy - H*dq;
    const Eigen::Vector<double, 4> v = ddy + Kd*de + Kp*e;

    const Eigen::Matrix<double, 6, 6> M = Plant::M(q);
    const Eigen::Vector<double, 6> T = -Plant::D(q) - Plant::C(q, dq)*dq;

    const Eigen::Matrix<double, 2, 2> M11 = M.block(0, 0, 2, 2);
    const Eigen::Matrix<double, 2, 4> M12 = M.block(0, 2, 2, 4);
    const Eigen::Matrix<double, 4, 2> M21 = M.block(2, 0, 4, 2);
    const Eigen::Matrix<double, 4, 4> M22 = M.block(2, 2, 4, 4);
    const Eigen::Vector<double, 2> T1 = T.block(0, 0, 2, 1);
    const Eigen::Vector<double, 4> T2 = T.block(2, 0, 4, 1);

    const Eigen::Matrix<double, 4, 2> H1 = H.block(0, 0, 4, 2);
    const Eigen::Matrix<double, 4, 4> H2 = H.block(0, 2, 4, 4);
    const Eigen::Matrix<double, 4, 6> dH = Eigen::Matrix<double, 4, 6>::Zero();
    const Eigen::Matrix<double, 4, 4> Hd = H2 - H1*M11.inverse()*M12;
    const Eigen::Matrix<double, 4, 4> Hdp = Hd.transpose()*((Hd*Hd.transpose()).inverse());

    const Eigen::Vector<double, 4> ddq2 = Hdp*(v - dH*dq - H1*M11.inverse()*T1);
    const Eigen::Vector<double, 2> ddq1 = M11.inverse()*(T1 - M12*ddq2);
    const Eigen::Vector<double, 4> u = M21*ddq1 + M22*ddq2 - T2;

    return u;
}

Simple2::PositionController::PositionController() : Controller{6, 2, 3, 2} {

}

Eigen::VectorX<double> Simple2::PositionController::calculate(const Eigen::VectorX<double> &state, const Eigen::VectorX<double> &trajectory) const {
    const Eigen::Vector<double, 6> q = state.segment(0, 6);
    const Eigen::Vector<double, 6> dq = state.segment(6, 6);
    const Eigen::Vector<double, 2> y = trajectory.segment(0, 2);
    const Eigen::Vector<double, 2> dy = trajectory.segment(2, 2);
    const Eigen::Vector<double, 2> ddy = trajectory.segment(4, 2);

    const double psi = q(5);

    const Eigen::Matrix2d Rz {
        { std::cos(psi), std::sin(psi)},
        {-std::sin(psi), std::cos(psi)}
    };

    const Eigen::Vector2d e = y - q.segment(0, 2);
    const Eigen::Vector2d de = dy - dq.segment(0, 2);
    const Eigen::Vector2d v = Rz*(ddy + Kd*de + Kp*e);

    const double vddx = v(0);
    const double vddy = v(1);

    const double ref_phi = std::atan(-vddy/Params::g);
    const double ref_theta = std::atan(vddx*std::cos(ref_phi)/Params::g);

    return Eigen::Vector2d(ref_phi, ref_theta);
}

Simple2::Simple2() {
    drake::systems::DiagramBuilder<double> builder;

    auto sel1 = builder.AddSystem<Selector>(3*4, std::vector<int>{0, 1, 4, 5, 8, 9});
    auto position = builder.AddSystem<PositionController>();
    auto deriv = builder.AddSystem<drake::systems::DiscreteDerivative>(2, 0.01);
    auto zero = builder.AddSystem<drake::systems::ConstantVectorSource>(Eigen::Vector2d(0, 0));
    auto sel2 = builder.AddSystem<Selector>(3*4, std::vector<int>{2, 3});
    auto sel3 = builder.AddSystem<Selector>(3*4, std::vector<int>{6, 7});
    auto sel4 = builder.AddSystem<Selector>(3*4, std::vector<int>{10, 11});
    auto mux = builder.AddSystem<drake::systems::Multiplexer>(std::vector<int>{2, 2, 2, 2, 2, 2});
    auto hover = builder.AddSystem<HoverController>();
    auto conv = builder.AddSystem<Function>(4, 5, [](const Eigen::VectorX<double> &generalized) {return Plant::decodeControls(generalized);});

    builder.ExportInput(sel1->get_input_port(), "trajectory");
    builder.ExportInput(position->get_state_input_port(), "state");

    builder.ConnectInput("trajectory", sel2->get_input_port());
    builder.ConnectInput("trajectory", sel3->get_input_port());
    builder.ConnectInput("trajectory", sel4->get_input_port());
    builder.ConnectInput("state", hover->get_state_input_port());

    builder.Connect(sel1->get_output_port(), position->get_trajectory_input_port());
    builder.Connect(position->get_control_output_port(), deriv->get_input_port());
    builder.Connect(position->get_control_output_port(), mux->get_input_port(0));
    builder.Connect(sel2->get_output_port(), mux->get_input_port(1));
    builder.Connect(deriv->get_output_port(), mux->get_input_port(2));
    builder.Connect(sel3->get_output_port(), mux->get_input_port(3));
    builder.Connect(zero->get_output_port(), mux->get_input_port(4));
    builder.Connect(sel4->get_output_port(), mux->get_input_port(5));
    builder.Connect(mux->get_output_port(), hover->get_trajectory_input_port());
    builder.Connect(hover->get_control_output_port(), conv->get_input_port());

    builder.ExportOutput(conv->get_output_port(), "control");

    builder.BuildInto(this);
}

const drake::systems::InputPort<double> & Simple2::get_state_input_port() const {
    return GetInputPort("state");
}

const drake::systems::InputPort<double> & Simple2::get_trajectory_input_port() const {
    return GetInputPort("trajectory");
}

const drake::systems::OutputPort<double> & Simple2::get_control_output_port() const {
    return GetOutputPort("control");
}
