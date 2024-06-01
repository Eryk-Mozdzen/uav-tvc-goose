#pragma once

#include <drake/systems/framework/leaf_system.h>

class TrajectoryGenerator : public drake::systems::LeafSystem<double> {
    void EvalOutput(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const;

    virtual Eigen::VectorX<double> value(const double &time) const = 0;

public:
    TrajectoryGenerator(const int dim, const int derivatives_num);
};
