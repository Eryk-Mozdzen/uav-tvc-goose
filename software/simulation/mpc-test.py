import opengen as og
import numpy as np
import scipy.constants

HP = 20
HC = 10
T = 0.05

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

    Fxy = (z2 + scipy.constants.g)*np.sqrt(1 - np.cos(x[3])*np.cos(x[4]))/(np.cos(x[3])*np.cos(x[4]))

    dx = np.array([
        x[6],
        x[7],
        x[8],
        x[9],
        x[10],
        x[11],
        Fxy*np.cos(x[5])/m,
        Fxy*np.sin(x[5])/m,
        z2,
        phi2,
        theta2,
        psi2,
    ])

    return x + dx*T

def traj(u, u2):
    return np.array([
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

data = []
x = np.array([0, 0, 0, 0.1, -0.01, 0, 0, 0, 0, 0, 0, 0])

mng = og.tcp.OptimizerTcpManager('open_optimizer')
mng.start()

def trajectory(t):
    a = 1
    w = 1
    return np.array([
        #a*np.sin(t*w)*np.cos(t*w)/(np.sin(t*w)**2 + 1),
        #a*np.cos(t*w)/(np.sin(t*w)**2 + 1),
        #np.full_like(t, 1),
        #np.atan2(-a*w*np.sin(t*w)/(np.sin(t*w)**2 + 1) - 2*a*w*np.sin(t*w)*np.cos(t*w)**2/(np.sin(t*w)**2 + 1)**2, -a*w*np.sin(t*w)**2/(np.sin(t*w)**2 + 1) + a*w*np.cos(t*w)**2/(np.sin(t*w)**2 + 1) - 2*a*w*np.sin(t*w)**2*np.cos(t*w)**2/(np.sin(t*w)**2 + 1)**2),

        np.cos(t*w),
        np.sin(t*w),
        np.full_like(t, 1),
        t*w + np.full_like(t, np.pi/2),

        #np.full_like(t, -1),
        #np.full_like(t, 1),
        #np.full_like(t, 1),
        #np.full_like(t, np.pi/2),
    ])

for i in range(200):
    print(f't = {i}')

    x_tr = trajectory(T*np.arange(i, i+HP)).transpose()

    response = mng.call(x.tolist() + x_tr.flatten().tolist(), initial_guess=np.zeros(4*HC))

    if response.is_ok():
        u2 = np.array(response.get().solution).reshape(HC, 4)
    else:
        print(response.get().message)

    x_pred = np.zeros((HP+1, 12))
    x_pred[0] = x
    u = np.hstack([x[2:6], x[8:12], u2[0]])
    for k in range(0, HC):
        x_pred[k+1] = dynamics(x_pred[k], u)
        u = traj(u, u2[k])
    for k in range(HC, HP):
        x_pred[k+1] = dynamics(x_pred[k], u)
        u = traj(u, u2[-1])

    u = np.hstack([x[2:6], x[8:12], u2[0]])

    data.append({
        'x': x,
        'x_tr': x_tr,
        'u': u,
        'x_pred': x_pred
    })

    x = dynamics(x, u)

mng.kill()

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

fig = plt.figure()
gs = fig.add_gridspec(2, 2)

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

dir_xt = ax00.quiver(0, 0, 1, 0, angles='xy', scale_units='xy', scale=2, color='k', zorder=1)
line_tr, = ax00.plot([], [], 'k--', zorder=1)
line_pr, = ax00.plot([], [], 'b--', zorder=2)
dir_x = ax00.quiver(0, 0, 1, 0, angles='xy', scale_units='xy', scale=2, color='r', zorder=3)

line_z, = ax10.plot([], [], label='z')
line_zt, = ax10.plot([], [], 'k--', label='z trajectory')

line_phi, = ax01.plot([], [], label='phi')
line_phit, = ax01.plot([], [], 'k--', label='phi trajectory')
line_theta, = ax01.plot([], [], label='theta')
line_thetat, = ax01.plot([], [], 'k--', label='theta trajectory')
line_psi, = ax11.plot([], [], label='psi')
line_psit, = ax11.plot([], [], 'k--', label='psi trajectory')

ax10.legend()
ax01.legend()
ax11.legend()

ax01.set_xlim(0, len(data)*T)
ax10.set_xlim(0, len(data)*T)
ax11.set_xlim(0, len(data)*T)

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
    line_tr.set_data([], [])
    line_pr.set_data([], [])
    line_z.set_data([], [])
    line_zt.set_data([], [])
    line_phi.set_data([], [])
    line_phit.set_data([], [])
    line_theta.set_data([], [])
    line_thetat.set_data([], [])
    line_psi.set_data([], [])
    line_psit.set_data([], [])
    return dir_xt, dir_x, line_tr, line_pr, line_z, line_zt, line_phi, line_phit, line_theta, line_thetat, line_psi, line_psit,

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
        np.cos(d['x_tr'][0][3]),
        np.sin(d['x_tr'][0][3]),
    )

    dir_x.set_offsets([
        d['x'][0],
        d['x'][1],
    ])
    dir_x.set_UVC(
        np.cos(d['x'][5]),
        np.sin(d['x'][5]),
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
        [i[0] for i in u],
        [i[1][0] for i in u],
    )
    line_phi.set_data(
        [i[0] for i in x],
        [np.rad2deg(i[1][3]) for i in x],
    )
    line_phit.set_data(
        [i[0] for i in u],
        [np.rad2deg(i[1][1]) for i in u],
    )
    line_theta.set_data(
        [i[0] for i in x],
        [np.rad2deg(i[1][4]) for i in x],
    )
    line_thetat.set_data(
        [i[0] for i in u],
        [np.rad2deg(i[1][2]) for i in u],
    )
    line_psi.set_data(
        [i[0] for i in x],
        [np.rad2deg(i[1][5]) for i in x],
    )
    line_psit.set_data(
        [i[0] for i in u],
        [np.rad2deg(i[1][3]) for i in u],
    )

    return dir_xt, dir_x, line_tr, line_pr, line_z, line_zt, line_phi, line_phit, line_theta, line_thetat, line_psi, line_psit,

anim = FuncAnimation(
    fig=fig,
    func=update,
    frames=len(data),
    init_func=init,
    interval=T*1000,
    blit=True,
)

plt.show()
