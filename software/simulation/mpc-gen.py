import casadi as cs
import opengen as og
import dynamics

HP = 20
HC = 5
T = 0.05

NX = 14
NT = 8
NU = 4

H = cs.DM([
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0],
])

Z = cs.DM([
    [0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0],
])

QH = cs.DM.eye(NT)*100
QZ = cs.DM.eye(4)*1

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
    x = dynamics.dynamics(x, v[t], T)
    constraints = cs.vertcat(constraints, x[13])
for t in range(HC, HP):
    dx = cs.mtimes([H, x]) - x_tr[t]
    dz = cs.mtimes([Z, x])
    cost +=cs.mtimes([dx.T, QH, dx])
    cost +=cs.mtimes([dz.T, QZ, dz])
    x = dynamics.dynamics(x, v[-1], T)
    constraints = cs.vertcat(constraints, x[13])

variables = cs.vertcat(*v)
parameters = cs.vertcat(x_0, *x_tr)

v_bounds = og.constraints.Rectangle(
    [-10, -10, -10, -10]*HC,
    [+10, +10, +10, +10]*HC,
)

x_bounds = og.constraints.Rectangle(
    [0   ]*HP,
    [1000]*HP,
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
