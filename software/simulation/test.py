import opengen as og
import numpy as np

N = 100
T = 0.02
x = np.array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
u = np.array([1000, 0, 0, 0, 0])
U = np.tile(u, (N, 1))

def dynamics_continuous(x, u):
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

    return np.array([
        #vx*np.cos(theta)*np.cos(psi) + vy*(np.sin(phi)*np.sin(theta)*np.cos(psi) - np.cos(phi)*np.sin(psi)) + vz*(np.cos(phi)*np.sin(theta)*np.cos(psi) + np.sin(phi)*np.sin(psi)),
        #vx*np.cos(theta)*np.sin(psi) + vy*(np.sin(phi)*np.sin(theta)*np.sin(psi) + np.cos(phi)*np.cos(psi)) + vz*(np.cos(phi)*np.sin(theta)*np.sin(psi) - np.sin(phi)*np.cos(psi)),
        #-vx*np.sin(theta) + vy*np.sin(phi)*np.cos(theta) + vz*np.cos(phi)*np.cos(theta),
        #wx + wy*np.sin(phi)*np.tan(theta) + wz*np.cos(phi)*np.tan(theta),
        #wy*np.cos(phi) - wz*np.sin(phi),
        #wy*(np.sin(phi)/np.cos(theta)) + wz*(np.cos(phi)/np.cos(theta)),
        #(1/m)*(F2 - F4) + g*np.sin(theta),
        #(1/m)*(F3 - F1) - g*np.sin(phi)*np.cos(theta),
        #(1/m)*Ft - g*np.cos(phi)*np.cos(theta),
        #(1/J_xx)*(J_yy - J_zz)*wy*wz + (1/J_xx)*l*(F3 - F1) + (J_r/J_xx)*wr*wy,
        #(1/J_yy)*(J_zz - J_xx)*wx*wz + (1/J_yy)*l*(F4 - F2) - (J_r/J_yy)*wr*wx,
        #(1/J_zz)*(J_xx - J_yy)*wx*wy - (1/J_zz)*r*(F1 + F2 + F3 + F4 + 4*Fs) + (1/J_zz)*Mr,
        vx,
        vy,
        vz,
        wx,
        wy,
        wz,
        (1/m)*(F2 - F4),
        (1/m)*(F3 - F1),
        (1/m)*Ft - g,
        (1/J_xx)*l*(F3 - F1),
        (1/J_yy)*l*(F4 - F2),
        - (1/J_zz)*r*(F1 + F2 + F3 + F4),
    ])

def dynamics_discrete(x, u):
    dx = dynamics_continuous(x, u)
    return x + T*dx

data = []

mng = og.tcp.OptimizerTcpManager('open_optimizer')
mng.start()

def trajectory(t):
    a = 1
    w = 0.5
    return np.array([
        a*np.cos(w*t)*np.sin(w*t)/(1 + np.sin(w*t)**2),
        a*np.cos(w*t)/(1 + np.sin(w*t)**2),
        np.full_like(t, 1),
        np.arctan2(
            -(5 + np.cos(2*w*t))*np.sin(w*t),
            -1 + 3*np.cos(2*w*t),
        ),
    ])

for i in range(1000):
    x_tr = trajectory(T*np.arange(i, i+N)).transpose()

    response = mng.call(u.tolist() + x.tolist() + x_tr.flatten().tolist(), initial_guess=[1000, 0, 0, 0, 0]*N)

    if response.is_ok():
        U = np.array(response.get().solution).reshape(-1, 5)
        u = U[0]
    else:
        print(response.get().message)

    x_pred = np.zeros((N+1, 12))
    x_pred[0] = x
    for k in range(N):
        x_pred[k+1] = dynamics_discrete(x_pred[k], U[k])

    data.append({
        'x': x,
        'x_tr': x_tr,
        'u': u,
        'x_pred': x_pred
    })

    x = dynamics_discrete(x, u)

mng.kill()

import matplotlib
matplotlib.use('qt5agg')
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

fig = plt.figure()
gs = fig.add_gridspec(2, 2)

ax = fig.add_subplot(gs[:, :1])
ax.set_xlim(-2, 2)
ax.set_ylim(-2, 2)
ax.set_xlabel('x')
ax.set_ylabel('y')
ax.set_aspect(1)
ax.grid()

ax2 = fig.add_subplot(gs[0, 1])
ax2.set_ylim(0, 2000)
ax2.set_ylabel('rotor [rad/s]')
ax2.grid()

ax3 = fig.add_subplot(gs[1, 1])
ax3.set_ylim(-15, 15)
ax3.set_xlabel('t')
ax3.set_ylabel('angle [deg]')
ax3.grid()

line_xt, = ax.plot([], [], 'k*')
line_tr, = ax.plot([], [], 'k--')
line_pr, = ax.plot([], [], 'b--')
line_x, = ax.plot([], [], 'r*')

line_u1, = ax2.plot([], [])
line_u2, = ax3.plot([], [])
line_u3, = ax3.plot([], [])
line_u4, = ax3.plot([], [])
line_u5, = ax3.plot([], [])

u = []

def init():
    u.clear()
    line_tr.set_data([], [])
    line_pr.set_data([], [])
    line_x.set_data([], [])
    line_xt.set_data([], [])
    line_u1.set_data([], [])
    line_u2.set_data([], [])
    line_u3.set_data([], [])
    line_u4.set_data([], [])
    line_u5.set_data([], [])
    return line_tr, line_pr, line_x, line_u1, line_u2, line_u3, line_u4, line_u5,

def update(frame):
    d = data[frame]

    u.append((frame*T, d['u']))

    line_tr.set_data(
        [d['x_tr'][i][0] for i in range(N)],
        [d['x_tr'][i][1] for i in range(N)],
    )
    line_xt.set_data(
        [d['x_tr'][0][0]],
        [d['x_tr'][0][1]],
    )
    line_pr.set_data(
        [d['x_pred'][i][0] for i in range(N)],
        [d['x_pred'][i][1] for i in range(N)],
    )
    line_x.set_data(
        [d['x'][0]],
        [d['x'][1]],
    )
    line_u1.set_data(
        [i[0] for i in u],
        [i[1][0] for i in u],
    )
    line_u2.set_data(
        [i[0] for i in u],
        [np.rad2deg(i[1][1]) for i in u],
    )
    line_u3.set_data(
        [i[0] for i in u],
        [np.rad2deg(i[1][2]) for i in u],
    )
    line_u4.set_data(
        [i[0] for i in u],
        [np.rad2deg(i[1][3]) for i in u],
    )
    line_u5.set_data(
        [i[0] for i in u],
        [np.rad2deg(i[1][4]) for i in u],
    )

    ax2.set_xlim(frame*T-10, frame*T)
    ax3.set_xlim(frame*T-10, frame*T)

    return line_tr, line_pr, line_x, line_u1, line_u2, line_u3, line_u4, line_u5,

anim = FuncAnimation(
    fig=fig,
    func=update,
    frames=len(data),
    init_func=init,
    interval=T*1000,
    blit=True,
)

plt.show()
