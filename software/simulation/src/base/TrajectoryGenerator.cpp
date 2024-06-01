#include "TrajectoryGenerator.h"

TrajectoryGenerator::TrajectoryGenerator(const int dim, const int derivatives_num) {
    DeclareVectorOutputPort("trajectory", dim*derivatives_num, &TrajectoryGenerator::EvalOutput);
}

void TrajectoryGenerator::EvalOutput(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const {
    const double &time = context.get_time();

    const Eigen::VectorX<double> trajectory = value(time);

    output->SetFromVector(trajectory);
}
