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

def dynamics(x, u):
    k0 = 2
    k1 = 3
    m  = 0.518

    v1 = u[8]  - k1*(x[10] - u[4]) - k0*(x[3] - u[0])
    v2 = u[9]  - k1*(x[11] - u[5]) - k0*(x[4] - u[1])
    v3 = u[10] - k1*(x[12] - u[6]) - k0*(x[5] - u[2])
    v4 =                     u[7]  - k0*(x[6] - u[3])

    ddx = (x[6]/m)*(cs.cos(x[3])*cs.sin(x[4])*cs.cos(x[5]) + cs.sin(x[3])*cs.sin(x[5]))
    ddy = (x[6]/m)*(cs.cos(x[3])*cs.sin(x[4])*cs.sin(x[5]) - cs.sin(x[3])*cs.cos(x[5]))
    ddz = (x[6]/m)*(cs.cos(x[3])*cs.cos(x[4])) - scipy.constants.g

    dx = cs.vcat([
        x[7],
        x[8],
        x[9],
        x[10],
        x[11],
        x[12],
        v4,
        ddx,
        ddy,
        ddz,
        v1,
        v2,
        v3,
    ])

    return x + dx*T

def control(u, uhd):
    return cs.vcat([
        u[0] + u[4]*T + 0.5*uhd[0]*T**2,
        u[1] + u[5]*T + 0.5*uhd[1]*T**2,
        u[2] + u[6]*T + 0.5*uhd[2]*T**2,
        u[3] + uhd[3]*T,

        u[4] + uhd[0]*T,
        u[5] + uhd[1]*T,
        u[6] + uhd[2]*T,
        uhd[3],

        uhd[0],
        uhd[1],
        uhd[2],
    ])

x_0 = cs.MX.sym('x_0', NX)
x_tr = [cs.MX.sym('x_tr_' + str(i), NT) for i in range(HP)]
uhd = [cs.MX.sym('uhd_' + str(i), NU) for i in range(HC)]

cost = 0

x = x_0
u = cs.vcat([x[3], x[4], x[5], x[6], x[10], x[11], x[12], uhd[0][3], uhd[0][0], uhd[0][1], uhd[0][2]])
for t in range(0, HC):
    dx = cs.mtimes([H, x]) - x_tr[t]
    dz = cs.mtimes([Z, x])
    cost +=cs.mtimes([dx.T, QH, dx])
    cost +=cs.mtimes([dz.T, QZ, dz])
    x = dynamics(x, u)
    u = control(u, uhd[t])
for t in range(HC, HP):
    dx = cs.mtimes([H, x]) - x_tr[t]
    dz = cs.mtimes([Z, x])
    cost +=cs.mtimes([dx.T, QH, dx])
    cost +=cs.mtimes([dz.T, QZ, dz])
    x = dynamics(x, u)
    u = control(u, uhd[-1])

variables = cs.vertcat(*uhd)
parameters = cs.vertcat(x_0, *x_tr)

bounds = og.constraints.Rectangle(
    [-10, -10, -10, -10]*HC,
    [+10, +10, +10, +10]*HC,
)

problem = og.builder.Problem(variables, parameters, cost) \
    .with_constraints(bounds)

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
