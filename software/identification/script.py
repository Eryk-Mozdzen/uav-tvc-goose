import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import scipy.optimize
import scipy.constants

F_experiment = pd.read_csv('thrust_data.csv')
M_experiment = pd.read_csv('torque_data.csv')

F_u = F_experiment['throttle'].values
load = F_experiment['load'].values
F_w = F_experiment['velocity'].values
F_f = [(l - min(load))*scipy.constants.g for l in load]
F_v = F_experiment['voltage'].values
F_i = F_experiment['current'].values

arm = 0.03
M_u = M_experiment['throttle'].values
load = M_experiment['load'].values
M_m = [(l - min(load))*scipy.constants.g*arm for l in load]

def cost_function(theta, Fu, Fw, Ff, Mu, Mm):
    [Kw1, Kw2, Kf, Km] = theta
    cost  = np.sum((Fw - Kw1*Fu**Kw2)**2)
    cost += np.sum((Ff - Kf*Fw**2)**2)
    Mw = Kw1*Mu**Kw2
    cost += np.sum((Mm - Km*Mw**2)**2)
    return cost

[Kw1, Kw2, Kf, Km] = scipy.optimize.minimize(
    fun=cost_function,
    x0=[0, 1, 0, 0],
    args=(F_u, F_w, F_f, M_u, M_m),
    bounds=[(0, np.inf), (0, np.inf), (0, np.inf), (0, np.inf)],
).x

print(f'w(u) = {Kw1:e} u ^ {Kw2:e}')
print(f'F(w) = {Kf:e} w ^ 2')
print(f'M(w) = {Km:e} w ^ 2')

fig, axs = plt.subplots(2, 3)
axs = axs.flatten()
fig.delaxes(axs[3])

axs[0].scatter(100*F_u, F_w, label='samples', color='black', s=10)
u = np.linspace(min(F_u), max(F_u), 100)
axs[0].plot(100*u, Kw1*u**Kw2, label='best fit', color='red')
axs[0].set_xlabel('throttle [%]')
axs[0].set_ylabel('rotor velocity [rad/s]')
axs[0].set_title('rotor velocity vs. throttle')
axs[0].grid()
axs[0].legend()

axs[1].scatter(F_w, F_f, label='samples', color='black', s=10)
w = np.linspace(min(F_w), max(F_w), 100)
axs[1].plot(w, Kf*w**2, label='best fit', color='red')
axs[1].set_xlabel('rotor velocity [rad/s]')
axs[1].set_ylabel('thrust [N]')
axs[1].set_title('thrust vs. rotor velocity')
axs[1].grid()
axs[1].legend()

axs[2].scatter(Kw1*M_u**Kw2, M_m, label='est. samples', color='black', s=10)
w = np.linspace(min(Kw1*M_u**Kw2), max(Kw1*M_u**Kw2), 100)
axs[2].plot(w, Km*w**2, label='best fit', color='red')
axs[2].set_xlabel('rotor velocity [rad/s]')
axs[2].set_ylabel('torque [Nm]')
axs[2].set_title('torque vs. rotor velocity')
axs[2].grid()
axs[2].legend()

axs[4].scatter(100*F_u, F_f, color='black', s=10)
axs[4].set_xlabel('throttle [%]')
axs[4].set_ylabel('thrust [N]')
axs[4].set_title('thrust vs. throttle')
axs[4].grid()

axs[5].scatter(100*M_u, M_m, color='black', s=10)
axs[5].set_xlabel('throttle [%]')
axs[5].set_ylabel('torque [Nm]')
axs[5].set_title('torque vs. throttle')
axs[5].grid()

plt.figure()
plt.scatter(100*F_u, F_f/(F_i*F_v), color='black', s=10)
plt.xlabel('throttle [%]')
plt.ylabel('thrust / power [N/W]')
plt.title('motor-propeller efficiency vs. throttle')
plt.grid()

plt.show()
