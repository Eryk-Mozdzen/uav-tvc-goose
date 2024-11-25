import casadi as cs
import opengen as og
import numpy as np

HP = 100
HC = 80
T = 0.01

NX = 12
NT = 6
NU = 5

H = cs.DM([
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0],
])

Q = cs.diag([10, 10, 10, 100, 100, 10])
R = cs.diag([1, 1, 1, 1, 1])

def dynamics(x, u):
    phi = x[3]
    theta = x[4]
    psi = x[5]
    vx = x[6]
    vy = x[7]
    vz = x[8]
    wx = x[9]
    wy = x[10]
    wz = x[11]

    wr = u[0]
    a1 = u[1]
    a2 = u[2]
    a3 = u[3]
    a4 = u[4]

    g = 9.8065
    m = 0.332
    l = 0.0500
    r = 0.0665
    J_xx = 0.0009887
    J_yy = 0.0009817
    J_zz = 0.0002177
    J_r = 0.00001366
    a_s = -0.08727
    K_l = 0.4203
    K_m = 0.000000041
    K_w = 0.000003133

    Ft = K_w*wr**2
    F1 = K_l*Ft*a1
    F2 = K_l*Ft*a2
    F3 = K_l*Ft*a3
    F4 = K_l*Ft*a4
    Fs = K_l*Ft*a_s
    Mr = -K_m*wr**2

    return cs.vcat([
        vx*cs.cos(theta)*cs.cos(psi) + vy*(cs.sin(phi)*cs.sin(theta)*cs.cos(psi) - cs.cos(phi)*cs.sin(psi)) + vz*(cs.cos(phi)*cs.sin(theta)*cs.cos(psi) + cs.sin(phi)*cs.sin(psi)),
        vx*cs.cos(theta)*cs.sin(psi) + vy*(cs.sin(phi)*cs.sin(theta)*cs.sin(psi) + cs.cos(phi)*cs.cos(psi)) + vz*(cs.cos(phi)*cs.sin(theta)*cs.sin(psi) - cs.sin(phi)*cs.cos(psi)),
        -vx*cs.sin(theta) + vy*cs.sin(phi)*cs.cos(theta) + vz*cs.cos(phi)*cs.cos(theta),
        #vx,
        #vy,
        #vz,
        #wx + wy*cs.sin(phi)*cs.tan(theta) + wz*cs.cos(phi)*cs.tan(theta),
        #wy*cs.cos(phi) - wz*cs.sin(phi),
        #wy*(cs.sin(phi)/cs.cos(theta)) + wz*(cs.cos(phi)/cs.cos(theta)),
        wx,
        wy,
        wz,
        (1/m)*(F2 - F4) + g*cs.sin(theta),
        (1/m)*(F3 - F1) - g*cs.sin(phi)*cs.cos(theta),
        (1/m)*Ft - g*cs.cos(phi)*cs.cos(theta),
        #(1/m)*(F2 - F4),
        #(1/m)*(F3 - F1),
        #(1/m)*Ft - g,
        #(1/J_xx)*(J_yy - J_zz)*wy*wz + (1/J_xx)*l*(F3 - F1) + (J_r/J_xx)*wr*wy,
        #(1/J_yy)*(J_zz - J_xx)*wx*wz + (1/J_yy)*l*(F4 - F2) - (J_r/J_yy)*wr*wx,
        #(1/J_zz)*(J_xx - J_yy)*wx*wy - (1/J_zz)*r*(F1 + F2 + F3 + F4 + 4*Fs) + (1/J_zz)*Mr,
        (1/J_xx)*l*(F3 - F1),
        (1/J_yy)*l*(F4 - F2),
        - (1/J_zz)*r*(F1 + F2 + F3 + F4),
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
    cost +=cs.mtimes([dx.T, Q, dx])
    x = dynamics_discrete(x, u[t])
for t in range(HC, HP):
    dx = cs.mtimes([H, x]) - x_tr[t]
    cost +=cs.mtimes([dx.T, Q, dx])
    x = dynamics_discrete(x, u[-1])

du = u[0] - u_0
cost +=cs.mtimes([du.T, R, du])
for t in range(1, HC):
    du = u[t] - u[t-1]
    cost +=cs.mtimes([du.T, R, du])

variables = cs.vertcat(*u)
parameters = cs.vertcat(u_0, x_0, *x_tr)

bounds = og.constraints.Rectangle(
    [0, np.deg2rad(-10), np.deg2rad(-10), np.deg2rad(-10), np.deg2rad(-10)]*HC,
    [2000, np.deg2rad(10), np.deg2rad(10), np.deg2rad(10), np.deg2rad(10)]*HC,
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
