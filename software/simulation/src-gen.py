import os
import pickle
import sympy as sp

here = os.path.dirname(__file__)

with open(f'{here}/model/parameters.pkl', 'rb') as file:
    parameters = pickle.load(file)

with open(f'{here}/model/dynamics.pkl', 'rb') as file:
    dynamics = pickle.load(file)

with open(f'{here}/model/dynamics_linearized.pkl', 'rb') as file:
    dynamics_linearized = pickle.load(file)

with open(f'{here}/model/control_linearized.pkl', 'rb') as file:
    control_linearized = pickle.load(file)

X = sp.symbols('x1:15')
V = sp.symbols('v1:5')
U = sp.symbols('u1:5')

here = os.path.dirname(__file__)
os.makedirs(f'{here}/src/plant', exist_ok=True)

with open(f'{here}/src/plant/Plant.h', 'w') as file:
    file.write(
'''#pragma once

#include <drake/systems/framework/leaf_system.h>

class Plant : public drake::systems::LeafSystem<double> {
    void DoCalcTimeDerivatives(const drake::systems::Context<double> &context, drake::systems::ContinuousState<double> *derivatives) const;
    void eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const;

public:
'''
    )
    for param, value in parameters.items():
        file.write(f'    static constexpr double {sp.ccode(param)} = {value:e};\n')
    file.write(
'''
    Plant();

    const drake::systems::InputPort<double> & get_control_input_port() const;
    const drake::systems::OutputPort<double> & get_state_output_port() const;
};
'''
    )

with open(f'{here}/src/plant/Plant.cpp', 'w') as file:
    file.write(
'''#include "Plant.h"

Plant::Plant() {
'''
    )
    file.write('    this->DeclareContinuousState({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 500});\n')
    file.write('    this->DeclareVectorInputPort("u", ' + str(len(U)) + ');\n')
    file.write('    this->DeclareVectorOutputPort("x", ' + str(len(X)) + ', &Plant::eval, {this->all_state_ticket()});\n')
    file.write(
'''}

void Plant::DoCalcTimeDerivatives(const drake::systems::Context<double> &context, drake::systems::ContinuousState<double> *derivatives) const {
'''
    )
    file.write('    const Eigen::Vector<double, ' + str(len(U)) + '> u = this->GetInputPort("u").Eval(context);\n')
    file.write('    const Eigen::Vector<double, ' + str(len(X)) + '> x = context.get_continuous_state_vector().CopyToVector();\n')
    file.write('\n')
    for i, u in enumerate(U):
        if u in list(dynamics.free_symbols):
            file.write(f'    const double {sp.ccode(u)} = u[{i}];\n')
    file.write('\n')
    for i, x in enumerate(X):
        if x in list(dynamics.free_symbols):
            file.write(f'    const double {sp.ccode(x)} = x[{i}];\n')
    file.write('\n')
    for i, dyn in enumerate(dynamics):
        file.write(f'    derivatives->get_mutable_vector().SetAtIndex({i}, {sp.ccode(dyn)});\n')
    file.write(
'''}

void Plant::eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const {
    output->SetFrom(context.get_continuous_state_vector());
}

const drake::systems::InputPort<double> & Plant::get_control_input_port() const {
    return GetInputPort("u");
}

const drake::systems::OutputPort<double> & Plant::get_state_output_port() const {
    return GetOutputPort("x");
}
'''
    )

with open(f'{here}/src/controllers/PlantFeedbackLinearization.h', 'w') as file:
    file.write(
'''#pragma once

#include <drake/systems/framework/leaf_system.h>

class PlantFeedbackLinearization : public drake::systems::LeafSystem<double> {
    void eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const;

public:
    PlantFeedbackLinearization();
};
'''
    )

with open(f'{here}/src/controllers/PlantFeedbackLinearization.cpp', 'w') as file:
    file.write(
'''#include "PlantFeedbackLinearization.h"
#include "Plant.h"

PlantFeedbackLinearization::PlantFeedbackLinearization() {
'''
    )
    file.write('    this->DeclareVectorInputPort("v", ' + str(len(V)) + ');\n')
    file.write('    this->DeclareVectorInputPort("x", ' + str(len(X)) + ');\n')
    file.write('    this->DeclareVectorOutputPort("u", ' + str(len(U)) + ', &PlantFeedbackLinearization::eval);\n')
    file.write(
'''}

void PlantFeedbackLinearization::eval(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const {
'''
    )
    file.write('    const Eigen::Vector<double, ' + str(len(V)) + '> v = this->GetInputPort("v").Eval(context);\n')
    file.write('    const Eigen::Vector<double, ' + str(len(X)) + '> x = this->GetInputPort("x").Eval(context);\n')
    file.write('\n')
    for i, v in enumerate(V):
        if v in list(control_linearized.free_symbols):
            file.write(f'    const double {sp.ccode(v)} = v[{i}];\n')
    file.write('\n')
    for i, x in enumerate(X):
        if x in list(control_linearized.free_symbols):
            file.write(f'    const double {sp.ccode(x)} = x[{i}];\n')
    file.write('\n')
    for p, v in parameters.items():
        if p in list(control_linearized.free_symbols):
            file.write(f'    const double {sp.ccode(p)} = Plant::{sp.ccode(p)};\n')
    file.write('\n')
    for i, fl in enumerate(control_linearized):
        file.write(f'    output->SetAtIndex({i}, {sp.ccode(fl)});\n')
    file.write('}\n')

with open(f'{here}/dynamics.py', 'w') as file:
    file.write(
'''import casadi as cs

def dynamics(x, v, dt):
''')
    for p, v in parameters.items():
        if p in list(dynamics_linearized.free_symbols):
            file.write(f'    {sp.ccode(p)} = {v}\n')
    file.write('\n')
    for i, v in enumerate(V):
        if v in list(dynamics_linearized.free_symbols):
            file.write(f'    {sp.ccode(v)} = v[{i}]\n')
    file.write('\n')
    for i, x in enumerate(X):
        if x in list(dynamics_linearized.free_symbols):
            file.write(f'    {sp.ccode(x)} = x[{i}]\n')
    file.write('\n')
    file.write('    dx = cs.vcat([\n')
    for dx in dynamics_linearized:
        code = sp.pycode(dx)
        code = code.replace('math', 'cs')
        file.write(f'        {code},\n')
    file.write(
'''    ])

    return x + dt*dx
''')
