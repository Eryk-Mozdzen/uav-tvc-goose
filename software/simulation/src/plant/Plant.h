#pragma once

#include <drake/systems/framework/leaf_system.h>

class Plant : public drake::systems::LeafSystem<double> {
    static constexpr int dimQ = 6;

    static double current_w;

    using Q = Eigen::Vector<double, dimQ>;

    void DoCalcTimeDerivatives(const drake::systems::Context<double> &context, drake::systems::ContinuousState<double> *derivatives) const;
    void EvalOutput(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const;

    static Eigen::Matrix3d R(const Q &q);
    static Eigen::Matrix3d W(const Q &q);
    static Eigen::Matrix3d dR(const Q &q, const Q &dq);
    static Eigen::Matrix3d dW(const Q &q, const Q &dq);

public:
    Plant();

    static Eigen::Matrix<double, dimQ, dimQ> M(const Q &q);
    static Eigen::Matrix<double, dimQ, dimQ> C(const Q &q, const Q &dq);
    static Eigen::Vector<double, dimQ> D(const Q &q);

    static Eigen::Vector<double, 5> decodeControls(const Eigen::Vector<double, 4> &generalized);
    static Eigen::Vector<double, 4> encodeControls(const Eigen::Vector<double, 5> &real);
};
