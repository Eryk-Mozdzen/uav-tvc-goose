import opengen as og
import numpy as np
import scipy.constants

HP = 20
HC = 5
T = 0.05

def dynamics(x, v):
    m  = 0.518

    dx = np.array([
        x[7],
        x[8],
        x[9],
        x[10],
        x[11],
        x[12],
        v[3],
        (x[6]/m)*(np.cos(x[3])*np.sin(x[4])*np.cos(x[5]) + np.sin(x[3])*np.sin(x[5])),
        (x[6]/m)*(np.cos(x[3])*np.sin(x[4])*np.sin(x[5]) - np.sin(x[3])*np.cos(x[5])),
        (x[6]/m)*(np.cos(x[3])*np.cos(x[4])) - scipy.constants.g,
        v[0],
        v[1],
        v[2],
    ])

    return x + dx*T

data = []
x = np.array([0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0])

mng = og.tcp.OptimizerTcpManager('open_optimizer')
mng.start()

def trajectory(t):
    a = 1
    w = 1
    return np.array([
        a*np.sin(t*w)*np.cos(t*w)/(np.sin(t*w)**2 + 1),
        a*np.cos(t*w)/(np.sin(t*w)**2 + 1),
        np.full_like(t, 1),
        np.atan2(-a*w*np.sin(t*w)/(np.sin(t*w)**2 + 1) - 2*a*w*np.sin(t*w)*np.cos(t*w)**2/(np.sin(t*w)**2 + 1)**2, -a*w*np.sin(t*w)**2/(np.sin(t*w)**2 + 1) + a*w*np.cos(t*w)**2/(np.sin(t*w)**2 + 1) - 2*a*w*np.sin(t*w)**2*np.cos(t*w)**2/(np.sin(t*w)**2 + 1)**2),
        a*w*(1 - 3*np.sin(t*w)**2)/((np.sin(t*w)**2 + 1)**2),
        a*w*(np.sin(t*w)**2 - 3)*np.sin(t*w)/((np.sin(t*w)**2 + 1)**2),
        np.full_like(t, 0),
        -3*w*np.cos(t*w)/(np.sin(t*w)**2 + 1)

        #np.cos(t*w),
        #np.sin(t*w),
        #np.full_like(t, 1),
        #t*w + np.full_like(t, np.pi/2),
        #-w*np.sin(t*w),
        #w*np.cos(t*w),
        #np.full_like(t, 0),
        #np.full_like(t, w),

        #np.full_like(t, -1),
        #np.full_like(t, 1),
        #np.full_like(t, 1),
        #np.full_like(t, np.pi/2),
        #np.full_like(t, 0),
        #np.full_like(t, 0),
        #np.full_like(t, 0),
        #np.full_like(t, 0),
    ])

for i in range(500):
    print(f't = {i}')

    x_tr = trajectory(T*np.arange(i, i+HP)).transpose()

    response = mng.call(x.tolist() + x_tr.flatten().tolist())

    if response.is_ok():
        v = np.array(response.get().solution).reshape(HC, 4)
    else:
        print(response.get().message)

    x_pred = np.zeros((HP+1, 13))
    x_pred[0] = x
    for k in range(0, HC):
        x_pred[k+1] = dynamics(x_pred[k], v[k])
    for k in range(HC, HP):
        x_pred[k+1] = dynamics(x_pred[k], v[-1])

    data.append({
        'x': x,
        'x_tr': x_tr,
        'v': v[0],
        'x_pred': x_pred
    })

    x = dynamics(x, v[0])

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
line_tr, = ax00.plot([], [], 'k--', zorder=1, label='xy trajectory')
line_pr, = ax00.plot([], [], '--', zorder=2, label='xy')
dir_x = ax00.quiver(0, 0, 1, 0, angles='xy', scale_units='xy', scale=2, color='r', zorder=3)

line_z, = ax10.plot([], [], label='z')
line_zt, = ax10.plot([], [], 'k--', label='z trajectory')

line_phi, = ax01.plot([], [], label='phi')
line_theta, = ax01.plot([], [], label='theta')
line_psi, = ax11.plot([], [], label='psi')

ax00.legend()
ax10.legend()
ax01.legend()
ax11.legend()

ax01.set_xlim(0, len(data)*T)
ax10.set_xlim(0, len(data)*T)
ax11.set_xlim(0, len(data)*T)

x = []
tr = []
v = []

def init():
    x.clear()
    tr.clear()
    v.clear()
    dir_xt.set_offsets([0, 0])
    dir_xt.set_UVC(1, 0)
    dir_x.set_offsets([0, 0])
    dir_x.set_UVC(1, 0)
    line_tr.set_data([], [])
    line_pr.set_data([], [])
    line_z.set_data([], [])
    line_zt.set_data([], [])
    line_phi.set_data([], [])
    line_theta.set_data([], [])
    line_psi.set_data([], [])
    return dir_xt, dir_x, line_tr, line_pr, line_z, line_zt, line_phi, line_theta, line_psi,

def update(frame):
    d = data[frame]

    x.append((frame*T, d['x']))
    tr.append((frame*T, d['x_tr'][0]))
    v.append((frame*T, d['v']))

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

    return dir_xt, dir_x, line_tr, line_pr, line_z, line_zt, line_phi, line_theta, line_psi,

anim = FuncAnimation(
    fig=fig,
    func=update,
    frames=len(data),
    init_func=init,
    interval=T*1000,
    blit=True,
)

plt.show()
