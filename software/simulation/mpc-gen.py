import casadi as cs
import opengen as og
import scipy.constants

HP = 20
HC = 10
T = 0.05

NX = 12
NT = 4
NU = 4

H = cs.DM([
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0],
])

Z = cs.DM([
    [0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0],
])

QH = cs.DM.eye(NT)*100
QZ = cs.DM.eye(6)*1

def dynamics(x, u):
    z0_ref     = u[0]
    phi0_ref   = u[1]
    theta0_ref = u[2]
    psi0_ref   = u[3]
    z1_ref     = u[4]
    phi1_ref   = u[5]
    theta1_ref = u[6]
    psi1_ref   = u[7]
    z2_ref     = u[8]
    phi2_ref   = u[9]
    theta2_ref = u[10]
    psi2_ref   = u[11]

    Kp = 2
    Kd = 3
    m = 0.518

    z2     = z2_ref     + Kd*(z1_ref     - x[8])  + Kp*(z0_ref     - x[2])
    phi2   = phi2_ref   + Kd*(phi1_ref   - x[9])  + Kp*(phi0_ref   - x[3])
    theta2 = theta2_ref + Kd*(theta1_ref - x[10]) + Kp*(theta0_ref - x[4])
    psi2   = psi2_ref   + Kd*(psi1_ref   - x[11]) + Kp*(psi0_ref   - x[5])

    Fxy = (z2 + scipy.constants.g)*cs.sqrt(1 - cs.cos(x[3])*cs.cos(x[4]))/(cs.cos(x[3])*cs.cos(x[4]))

    dx = cs.vcat([
        x[6],
        x[7],
        x[8],
        x[9],
        x[10],
        x[11],
        Fxy*cs.cos(x[5])/m,
        Fxy*cs.sin(x[5])/m,
        z2,
        phi2,
        theta2,
        psi2,
    ])

    return x + dx*T

def traj(u, u2):
    return cs.vcat([
        u[0] + u[4]*T + 0.5*u2[0]*T**2,
        u[1] + u[5]*T + 0.5*u2[1]*T**2,
        u[2] + u[6]*T + 0.5*u2[2]*T**2,
        u[3] + u[7]*T + 0.5*u2[3]*T**2,
        u[4] + u2[0]*T,
        u[5] + u2[1]*T,
        u[6] + u2[2]*T,
        u[7] + u2[3]*T,
        u2[0],
        u2[1],
        u2[2],
        u2[3],
    ])

x_0 = cs.MX.sym('x_0', NX)
x_tr = [cs.MX.sym('x_tr_' + str(i), NT) for i in range(HP)]
u2 = [cs.MX.sym('u2_' + str(i), NU) for i in range(HC)]

cost = 0

x = x_0
u = cs.vcat([x_0[2:6], x_0[8:12], u2[0]])
for t in range(0, HC):
    dx = cs.mtimes([H, x]) - x_tr[t]
    dz = cs.mtimes([Z, x])
    cost +=cs.mtimes([dx.T, QH, dx])
    cost +=cs.mtimes([dz.T, QZ, dz])
    x = dynamics(x, u)
    u = traj(u, u2[t])
for t in range(HC, HP):
    dx = cs.mtimes([H, x]) - x_tr[t]
    dz = cs.mtimes([Z, x])
    cost +=cs.mtimes([dx.T, QH, dx])
    cost +=cs.mtimes([dz.T, QZ, dz])
    x = dynamics(x, u)
    u = traj(u, u2[-1])

variables = cs.vertcat(*u2)
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
