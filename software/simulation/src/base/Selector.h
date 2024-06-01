#pragma once

#include <drake/systems/framework/leaf_system.h>

template<typename T>
class Selector : public drake::systems::LeafSystem<T> {
    const std::vector<int> connections;

    void eval(const drake::systems::Context<T> &context, drake::systems::BasicVector<T> *output) const;

public:
    Selector(const int input_size, const std::vector<int> connections);
};

template<typename T>
Selector<T>::Selector(const int input_size, const std::vector<int> connections) : connections{connections} {
    this->DeclareVectorInputPort("U", input_size);
    this->DeclareVectorOutputPort("Y", connections.size(), &Selector<T>::eval);
}

template<typename T>
void Selector<T>::eval(const drake::systems::Context<T> &context, drake::systems::BasicVector<T> *output) const {
    const Eigen::VectorX<T> in = this->EvalVectorInput(context, 0)->CopyToVector();

    Eigen::VectorX<T> out(connections.size());

    for(unsigned int i=0; i<connections.size(); i++) {
        out(i) = in(connections[i]);
    }

    output->SetFromVector(out);
}
