import casadi as cs
import opengen as og
import scipy.constants

HP = 20
HC = 5
T = 0.05

NX = 13
NT = 8
NU = 4

H = cs.DM([
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
])

Z = cs.DM([
    [0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0],
])

QH = cs.DM.eye(NT)*100
QZ = cs.DM.eye(4)*1

def dynamics(x, v):
    m  = 0.518

    dx = cs.vcat([
        x[7],
        x[8],
        x[9],
        x[10],
        x[11],
        x[12],
        v[3],
        (x[6]/m)*(cs.cos(x[3])*cs.sin(x[4])*cs.cos(x[5]) + cs.sin(x[3])*cs.sin(x[5])),
        (x[6]/m)*(cs.cos(x[3])*cs.sin(x[4])*cs.sin(x[5]) - cs.sin(x[3])*cs.cos(x[5])),
        (x[6]/m)*(cs.cos(x[3])*cs.cos(x[4])) - scipy.constants.g,
        v[0],
        v[1],
        v[2],
    ])

    return x + dx*T

x_0 = cs.MX.sym('x_0', NX)
x_tr = [cs.MX.sym('x_tr_' + str(i), NT) for i in range(HP)]
v = [cs.MX.sym('v_' + str(i), NU) for i in range(HC)]

cost = 0
constraints = []

x = x_0
for t in range(0, HC):
    dx = cs.mtimes([H, x]) - x_tr[t]
    dz = cs.mtimes([Z, x])
    cost +=cs.mtimes([dx.T, QH, dx])
    cost +=cs.mtimes([dz.T, QZ, dz])
    x = dynamics(x, v[t])
    constraints = cs.vertcat(constraints, x[6])
for t in range(HC, HP):
    dx = cs.mtimes([H, x]) - x_tr[t]
    dz = cs.mtimes([Z, x])
    cost +=cs.mtimes([dx.T, QH, dx])
    cost +=cs.mtimes([dz.T, QZ, dz])
    x = dynamics(x, v[-1])
    constraints = cs.vertcat(constraints, x[6])

variables = cs.vertcat(*v)
parameters = cs.vertcat(x_0, *x_tr)

v_bounds = og.constraints.Rectangle(
    [-10, -10, -10, -10]*HC,
    [+10, +10, +10, +10]*HC,
)

x_bounds = og.constraints.Rectangle(
    [0 ]*HP,
    [10]*HP,
)

problem = og.builder.Problem(variables, parameters, cost)   \
    .with_constraints(v_bounds)                             \
    .with_aug_lagrangian_constraints(constraints, x_bounds)

meta = og.config.OptimizerMeta()

build = og.config.BuildConfiguration() \
    .with_build_mode('release')        \
    .with_build_c_bindings()           \
    .with_tcp_interface_config()

solver = og.config.SolverConfiguration() \
    .with_tolerance(1e-5)

builder = og.builder.OpEnOptimizerBuilder(
    problem,
    meta,
    build,
    solver,
)

builder.build()
