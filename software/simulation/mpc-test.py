import opengen as og
import numpy as np

HP = 100
HC = 80
T = 0.01

x = np.array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
u = np.array([567, 0, 0, 0])
U = np.tile(u, (HC, 1))

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

    gravity = np.array([0, 0, -g]).reshape(-1, 1)
    F_thrust = np.array([0, 0, Ft]).reshape(-1, 1)
    M_areo = np.array([0, 0, -Mz]).reshape(-1, 1)
    M_gyro = np.array([Jr*wr*wy, -Jr*wr*wx, 0]).reshape(-1, 1)
    F_vanes = np.array([
        -0.5*np.sqrt(3)*F1 + 0.5*np.sqrt(3)*F3,
        -0.5*F1 + F2 - 0.5*F3,
        0,
    ]).reshape(-1, 1)
    M_vanes = np.array([
        l*(-0.5*F1 + F2 - 0.5*F3),
        l*(0.5*np.sqrt(3)*F1 - 0.5*np.sqrt(3)*F3),
        -r*(F1 + F2 + F3 + 3*Fs),
    ]).reshape(-1, 1)

    J = np.diag([Jxx, Jyy, Jzz])
    W = np.array([
        [1, np.sin(phi)*np.tan(theta), np.cos(phi)*np.tan(theta)],
        [0, np.cos(phi), -np.sin(phi)],
        [0, np.sin(phi)/np.cos(theta), np.cos(phi)/np.cos(theta)],
    ])
    Rz = np.array([
        [np.cos(psi), -np.sin(psi), 0],
        [np.sin(psi),  np.cos(psi), 0],
        [0, 0, 1],
    ])
    Ry = np.array([
        [ np.cos(theta), 0, np.sin(theta)],
        [0, 1, 0],
        [-np.sin(theta), 0, np.cos(theta)],
    ])
    Rx = np.array([
        [1, 0, 0],
        [0, np.cos(phi), -np.sin(phi)],
        [0, np.sin(phi),  np.cos(phi)],
    ])
    R = Rz*Ry*Rx

    w = np.array([wx, wy, wz]).reshape(-1, 1)

    return np.vstack([
        x[6],
        x[7],
        x[8],
        W@w,
        R@(F_thrust + F_vanes)/m + gravity,
        np.linalg.inv(J)@(M_areo + M_gyro + M_vanes) - np.linalg.inv(J)@np.cross(w.flatten(), np.dot(J, w).flatten()).reshape(-1, 1),
    ]).flatten()

def dynamics_discrete(x, u):
    dx = dynamics(x, u)
    return x + T*dx

data = []

mng = og.tcp.OptimizerTcpManager('open_optimizer')
mng.start()

def trajectory(t):
    a = 1
    w = 1
    return np.array([
        #a*np.sin(t*w)*np.cos(t*w)/(np.sin(t*w)**2 + 1),
        #a*np.cos(t*w)/(np.sin(t*w)**2 + 1),
        #np.full_like(t, 1),
        #np.full_like(t, 0),
        #np.full_like(t, 0),
        #np.atan2(-a*w*np.sin(t*w)/(np.sin(t*w)**2 + 1) - 2*a*w*np.sin(t*w)*np.cos(t*w)**2/(np.sin(t*w)**2 + 1)**2, -a*w*np.sin(t*w)**2/(np.sin(t*w)**2 + 1) + a*w*np.cos(t*w)**2/(np.sin(t*w)**2 + 1) - 2*a*w*np.sin(t*w)**2*np.cos(t*w)**2/(np.sin(t*w)**2 + 1)**2),

        #np.cos(t*w),
        #np.sin(t*w),
        #np.full_like(t, 1),
        #np.full_like(t, 0),
        #np.full_like(t, 0),
        #t*w + np.full_like(t, np.pi/2),

        np.full_like(t, 1),
        np.full_like(t, 1),
        np.full_like(t, 1),
        np.full_like(t, 0),
        np.full_like(t, 0),
        np.full_like(t, np.pi/2),
    ])

for i in range(1000):
    print(f't = {i}')

    x_tr = trajectory(T*np.arange(i, i+HP)).transpose()

    #response = mng.call(u.tolist() + x.tolist() + x_tr.flatten().tolist(), initial_guess=np.vstack((U[1:], U[-1])).flatten().tolist())
    response = mng.call(u.tolist() + x.tolist() + x_tr.flatten().tolist(), initial_guess=[567, 0, 0, 0]*HC)

    if response.is_ok():
        U = np.array(response.get().solution).reshape(-1, 4)
        u = U[0]
    else:
        print(response.get().message)

    x_pred = np.zeros((HP+1, 12))
    x_pred[0] = x
    for k in range(0, HC):
        x_pred[k+1] = dynamics_discrete(x_pred[k], U[k])
    for k in range(HC, HP):
        x_pred[k+1] = dynamics_discrete(x_pred[k], U[-1])

    data.append({
        'x': x,
        'x_tr': x_tr,
        'u': u,
        'x_pred': x_pred
    })

    x = dynamics_discrete(x, u)

mng.kill()

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

fig = plt.figure()
gs = fig.add_gridspec(2, 3)

ax00 = fig.add_subplot(gs[0, 0])
ax00.set_xlim(-2, 2)
ax00.set_ylim(-2, 2)
ax00.set_xlabel('x [m]')
ax00.set_ylabel('y [m]')
ax00.set_aspect(1)
ax00.grid()

ax10 = fig.add_subplot(gs[1, 0])
ax10.set_ylim(-1, 2)
ax10.set_ylabel('z [m]')
ax10.set_xlabel('t [s]')
ax10.grid()

ax01 = fig.add_subplot(gs[0, 1])
ax01.set_ylim(-45, 45)
ax01.set_ylabel('attitude [deg]')
ax01.grid()

ax11 = fig.add_subplot(gs[1, 1])
ax11.set_ylim(-180, 180)
ax11.set_ylabel('attitude [deg]')
ax11.set_xlabel('t [s]')
ax11.grid()

ax02 = fig.add_subplot(gs[0, 2])
ax02.set_ylim(0, 2000)
ax02.set_ylabel('rotor [rad/s]')
ax02.grid()

ax12 = fig.add_subplot(gs[1, 2])
ax12.set_ylim(-15, 15)
ax12.set_ylabel('servos [deg]')
ax12.set_xlabel('t [s]')
ax12.grid()

dir_xt = ax00.quiver(0, 0, 1, 0, angles='xy', scale_units='xy', scale=2, color='k', zorder=1)
line_tr, = ax00.plot([], [], 'k--', zorder=1)
line_pr, = ax00.plot([], [], 'b--', zorder=2)
dir_pr = [ax00.quiver(0, 0, 1, 0, angles='xy', scale_units='xy', scale=10, color='b', zorder=2) for i in range(10)]
dir_x = ax00.quiver(0, 0, 1, 0, angles='xy', scale_units='xy', scale=2, color='r', zorder=3)

line_z, = ax10.plot([], [], label='z')
line_zt, = ax10.plot([], [], 'k--', label='z trajectory')

line_phi, = ax01.plot([], [], label='phi')
line_theta, = ax01.plot([], [], label='theta')
line_psi, = ax11.plot([], [], label='psi')
line_psit, = ax11.plot([], [], 'k--', label='psi trajectory')

line_u1, = ax02.plot([], [])
line_u2, = ax12.plot([], [])
line_u3, = ax12.plot([], [])
line_u4, = ax12.plot([], [])

ax10.legend()
ax01.legend()
ax11.legend()

ax01.set_xlim(0, len(data)*T)
ax10.set_xlim(0, len(data)*T)
ax11.set_xlim(0, len(data)*T)
ax02.set_xlim(0, len(data)*T)
ax12.set_xlim(0, len(data)*T)

x = []
tr = []
u = []

def init():
    x.clear()
    tr.clear()
    u.clear()
    dir_xt.set_offsets([0, 0])
    dir_xt.set_UVC(1, 0)
    dir_x.set_offsets([0, 0])
    dir_x.set_UVC(1, 0)
    for dir in dir_pr:
        dir.set_offsets([0, 0])
        dir.set_UVC(1, 0)
    line_tr.set_data([], [])
    line_pr.set_data([], [])
    line_z.set_data([], [])
    line_zt.set_data([], [])
    line_phi.set_data([], [])
    line_theta.set_data([], [])
    line_psi.set_data([], [])
    line_psit.set_data([], [])
    line_u1.set_data([], [])
    line_u2.set_data([], [])
    line_u3.set_data([], [])
    line_u4.set_data([], [])
    return dir_xt, dir_x, *dir_pr, line_tr, line_pr, line_z, line_zt, line_phi, line_theta, line_psi, line_psit, line_u1, line_u2, line_u3, line_u4,

def update(frame):
    d = data[frame]

    x.append((frame*T, d['x']))
    tr.append((frame*T, d['x_tr'][0]))
    u.append((frame*T, d['u']))

    dir_xt.set_offsets([
        d['x_tr'][0][0],
        d['x_tr'][0][1],
    ])
    dir_xt.set_UVC(
        np.cos(d['x_tr'][0][5]),
        np.sin(d['x_tr'][0][5]),
    )

    dir_x.set_offsets([
        d['x'][0],
        d['x'][1],
    ])
    dir_x.set_UVC(
        np.cos(d['x'][5]),
        np.sin(d['x'][5]),
    )

    big = d['x_pred']
    small = dir_pr
    step = len(big) // len(small)
    evenly_spaced_big_array = big[::step]
    evenly_spaced_big_array = evenly_spaced_big_array[:len(small)]
    for dir, pred in zip(small, evenly_spaced_big_array):
        dir.set_offsets([
            pred[0],
            pred[1],
        ])
        dir.set_UVC(
            np.cos(pred[5]),
            np.sin(pred[5]),
        )

    line_tr.set_data(
        [d['x_tr'][i][0] for i in range(HP)],
        [d['x_tr'][i][1] for i in range(HP)],
    )
    line_pr.set_data(
        [d['x_pred'][i][0] for i in range(HP)],
        [d['x_pred'][i][1] for i in range(HP)],
    )
    line_z.set_data(
        [i[0] for i in x],
        [i[1][2] for i in x],
    )
    line_zt.set_data(
        [i[0] for i in tr],
        [i[1][2] for i in tr],
    )
    line_phi.set_data(
        [i[0] for i in x],
        [np.rad2deg(i[1][3]) for i in x],
    )
    line_theta.set_data(
        [i[0] for i in x],
        [np.rad2deg(i[1][4]) for i in x],
    )
    line_psi.set_data(
        [i[0] for i in x],
        [np.rad2deg(i[1][5]) for i in x],
    )
    line_psit.set_data(
        [i[0] for i in tr],
        [np.rad2deg(i[1][5]) for i in tr],
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

    return dir_xt, dir_x, *dir_pr, line_tr, line_pr, line_z, line_zt, line_phi, line_theta, line_psi, line_psit, line_u1, line_u2, line_u3, line_u4,

anim = FuncAnimation(
    fig=fig,
    func=update,
    frames=len(data),
    init_func=init,
    interval=T*1000,
    blit=True,
)

plt.show()
