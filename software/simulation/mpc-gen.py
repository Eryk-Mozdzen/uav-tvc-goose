import casadi as cs
import opengen as og
import numpy as np

HP = 100
HC = 80
T = 0.01

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
])

QT = cs.diag([100, 100, 100, 100])
QZ = cs.diag([100, 100])
R = cs.diag([0.1, 1, 1, 1])

def dynamics(x, u):
    Kf  = 1.458825e-05
    Km  = 2.531647e-07
    Kl  = 0.34802890073780146
    Jxx = 3513658.12176*1e-9
    Jyy = 4068713.21612*1e-9
    Jzz = 3724881.39219*1e-9
    Jr  =   38872.17503*1e-9
    m   = 0.518
    l   = 62.89435*1e-3
    r   = 0.08182101368679512
    a0  = np.radians(-10)
    g   = 9.8065

    phi = x[3]
    theta = x[4]
    psi = x[5]
    wx = x[9]
    wy = x[10]
    wz = x[11]

    wr = u[0]
    a1 = u[1]
    a2 = u[2]
    a3 = u[3]

    Ft = Kf*wr**2
    Mz = Km*wr**2
    F1 = Kl*Ft*a1
    F2 = Kl*Ft*a2
    F3 = Kl*Ft*a3
    Fs = Kl*Ft*a0

    gravity = cs.vcat([0, 0, -g])
    F_thrust = cs.vcat([0, 0, Ft])
    M_areo = cs.vcat([0, 0, -Mz])
    M_gyro = cs.vcat([Jr*wr*wy, -Jr*wr*wx, 0])
    F_vanes = cs.vcat([
        -0.5*cs.sqrt(3)*F1 + 0.5*cs.sqrt(3)*F3,
        -0.5*F1 + F2 - 0.5*F3,
        0,
    ])
    M_vanes = cs.vcat([
        l*(-0.5*F1 + F2 - 0.5*F3),
        l*(0.5*cs.sqrt(3)*F1 - 0.5*cs.sqrt(3)*F3),
        -r*(F1 + F2 + F3 + 3*Fs),
    ])

    J = cs.diag([Jxx, Jyy, Jzz])
    J_inv = cs.DM(np.linalg.inv(np.array(J)))
    W = cs.vcat([
        cs.hcat([1, cs.sin(phi)*cs.tan(theta), cs.cos(phi)*cs.tan(theta)]),
        cs.hcat([0, cs.cos(phi), -cs.sin(phi)]),
        cs.hcat([0, cs.sin(phi)/cs.cos(theta), cs.cos(phi)/cs.cos(theta)]),
    ])
    Rz = cs.vcat([
        cs.hcat([cs.cos(psi), -cs.sin(psi), 0]),
        cs.hcat([cs.sin(psi),  cs.cos(psi), 0]),
        cs.hcat([0, 0, 1]),
    ])
    Ry = cs.vcat([
        cs.hcat([ cs.cos(theta), 0, cs.sin(theta)]),
        cs.hcat([0, 1, 0]),
        cs.hcat([-cs.sin(theta), 0, cs.cos(theta)]),
    ])
    Rx = cs.vcat([
        cs.hcat([1, 0, 0]),
        cs.hcat([0, cs.cos(phi), -cs.sin(phi)]),
        cs.hcat([0, cs.sin(phi),  cs.cos(phi)]),
    ])
    R = cs.mtimes([Rz, Ry, Rx])

    w = cs.vcat([wx, wy, wz])

    return cs.vcat([
        x[6],
        x[7],
        x[8],
        cs.mtimes([W, w]),
        cs.mtimes([R, F_thrust + F_vanes])/m + gravity,
        cs.mtimes([J_inv, M_areo + M_gyro + M_vanes]) - cs.mtimes([J_inv, cs.cross(w, cs.mtimes(J, w))]),
    ])

def dynamics_discrete(x, u):
    dx = dynamics(x, u)
    return x + T*dx

u_0 = cs.MX.sym('u_0', NU)
x_0 = cs.MX.sym('x_0', NX)
x_tr = [cs.MX.sym('x_tr_' + str(i), NT) for i in range(HP)]
u = [cs.MX.sym('u_' + str(i), NU) for i in range(HC)]

cost = 0

x = x_0
for t in range(0, HC):
    dx = cs.mtimes([H, x]) - x_tr[t]
    cost +=cs.mtimes([dx.T, QT, dx])
    dx = cs.mtimes([Z, x])
    cost +=cs.mtimes([dx.T, QZ, dx])
    x = dynamics_discrete(x, u[t])
for t in range(HC, HP):
    dx = cs.mtimes([H, x]) - x_tr[t]
    cost +=cs.mtimes([dx.T, QT, dx])
    dx = cs.mtimes([Z, x])
    cost +=cs.mtimes([dx.T, QZ, dx])
    x = dynamics_discrete(x, u[-1])

du = u[0] - u_0
cost +=cs.mtimes([du.T, R, du])
for t in range(1, HC):
    du = u[t] - u[t-1]
    cost +=cs.mtimes([du.T, R, du])

variables = cs.vertcat(*u)
parameters = cs.vertcat(u_0, x_0, *x_tr)

bounds = og.constraints.Rectangle(
    [400, np.deg2rad(-10), np.deg2rad(-10), np.deg2rad(-10)]*HC,
    [800, np.deg2rad(10), np.deg2rad(10), np.deg2rad(10)]*HC,
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
