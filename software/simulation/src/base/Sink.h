#pragma once

#include <fstream>
#include <drake/systems/framework/leaf_system.h>
#include <drake/systems/framework/diagram_builder.h>
#include <drake/systems/primitives/matrix_gain.h>

template<typename T>
class Sink : public drake::systems::LeafSystem<T> {
    std::unique_ptr<int> counter;
    std::unique_ptr<Eigen::MatrixX<T>> data;

	drake::systems::EventStatus eval(const drake::systems::Context<T> &context) const {
        Eigen::VectorX<T> row(data->cols());

        row[0] = context.get_time();

        int start = 1;
        for(int i=0; i<this->num_input_ports(); i++) {
            const Eigen::VectorX<T> signal = this->get_input_port(i).Eval(context);

            row.segment(start, signal.size()) = signal;
            start +=signal.size();
        }

        if(*counter>=data->rows()) {
            data->conservativeResize(2*data->rows(), data->cols());
        }

        data->row(*counter) = row;
        (*counter)++;

        return drake::systems::EventStatus::Succeeded();
	}

public:
	Sink() {
        this->DeclarePerStepPublishEvent(&Sink::eval);

        counter = std::make_unique<int>();
        data = std::make_unique<Eigen::MatrixX<T>>(1, 1);

        *counter = 0;
	}

    ~Sink() {
        std::ofstream file("output.csv");
        file << "t";
        for(int i=0; i<this->num_input_ports(); i++) {
            file << "," << this->get_input_port(i).get_name();
        }
        file << "\n";
        file << data->topRows(*counter).format(Eigen::IOFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, ","));
    }

    void Connect(drake::systems::DiagramBuilder<T>* builder, const drake::systems::OutputPort<T> &signal, const std::string &name) {
        this->DeclareVectorInputPort(name, signal.size());

        builder->Connect(signal, this->GetInputPort(name));

        data->conservativeResize(data->rows(), data->cols() + signal.size());
    }

    void Connect(drake::systems::DiagramBuilder<T>* builder, const drake::systems::OutputPort<T> &signal, const std::vector<int> indices, const std::string &name) {
        Eigen::MatrixXd matrix = Eigen::MatrixXd::Zero(indices.size(), signal.size());
        for(unsigned int i=0; i<indices.size(); i++) {
            matrix(i, indices[i]) = 1;
        }

        auto selector = builder->template AddSystem<drake::systems::MatrixGain>(matrix);

        builder->Connect(signal, selector->get_input_port());

        Connect(builder, selector->get_output_port(), name);
    }
};
