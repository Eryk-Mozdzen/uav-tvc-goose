#pragma once

#include <drake/systems/framework/leaf_system.h>

template<typename T>
class Function : public drake::systems::LeafSystem<T> {
    const std::function<Eigen::VectorX<T>(const Eigen::VectorX<T> &)> func;

    void eval(const drake::systems::Context<T> &context, drake::systems::BasicVector<T> *output) const;

public:
    Function(const int input_size, const int output_size, const std::function<Eigen::VectorX<T>(const Eigen::VectorX<T> &)> func);
};

template<typename T>
Function<T>::Function(const int input_size, const int output_size, const std::function<Eigen::VectorX<T>(const Eigen::VectorX<T> &)> func) : func{func} {
    this->DeclareVectorInputPort("U", input_size);
    this->DeclareVectorOutputPort("Y", output_size, &Function<T>::eval);
}

template<typename T>
void Function<T>::eval(const drake::systems::Context<T> &context, drake::systems::BasicVector<T> *output) const {
    const Eigen::VectorX<T> in = this->EvalVectorInput(context, 0)->CopyToVector();

    const Eigen::VectorX<T> out = func(in);

    output->SetFromVector(out);
}
