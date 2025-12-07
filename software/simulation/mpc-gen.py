import casadi as cs
import opengen as og
import dynamics

HP = 20
HC = 5
T = 0.05

NX = 14
NT = 12
NU = 4

IX = [0, 1, 2, 5]
IZ = [3, 4]
QX = 1000
QZ = 1

x_0 = cs.MX.sym('x_0', NX)
x_tr = [cs.MX.sym('x_tr_' + str(i), NT) for i in range(HP)]
v = [cs.MX.sym('v_' + str(i), NU) for i in range(HC)]

cost = 0
constraints = []

x = x_0
for t in range(0, HC):
    dx = dynamics.dynamics(x, v[t])
    for i, idx in enumerate(IX):
        cost +=QX*( x[0+idx] - x_tr[t][0+i])**2
        cost +=QX*( x[7+idx] - x_tr[t][4+i])**2
        cost +=QX*(dx[7+idx] - x_tr[t][8+i])**2
    for idx in IZ:
        cost +=QZ* x[0+idx]**2
        cost +=QZ* x[7+idx]**2
        cost +=QZ*dx[7+idx]**2
    x +=T*dx
    constraints.append(x[13])
for t in range(HC, HP):
    dx = dynamics.dynamics(x, v[-1])
    for i, idx in enumerate(IX):
        cost +=QX*( x[0+idx] - x_tr[t][0+i])**2
        cost +=QX*( x[7+idx] - x_tr[t][4+i])**2
        cost +=QX*(dx[7+idx] - x_tr[t][8+i])**2
    for idx in IZ:
        cost +=QZ* x[0+idx]**2
        cost +=QZ* x[7+idx]**2
        cost +=QZ*dx[7+idx]**2
    x +=T*dx
    constraints.append(x[13])

variables = cs.vertcat(*v)
parameters = cs.vertcat(x_0, *x_tr)
constraints = cs.vertcat(*constraints)

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
    .with_tolerance(1e-6)

builder = og.builder.OpEnOptimizerBuilder(
    problem,
    meta,
    build,
    solver,
)

builder.build()
