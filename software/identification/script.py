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

arm = 0.03
M_u = M_experiment['throttle'].values
load = M_experiment['load'].values
M_m = [(l - min(load))*scipy.constants.g*arm for l in load]

def cost_function(theta, Fu, Fw, Ff, Mu, Mm):
    [K_w, m_w, K_f, K_m] = theta
    cost  = np.sum((Fw - K_w*Fu**m_w)**2)
    cost += np.sum((Ff - K_f*Fw**2)**2)
    cost += np.sum((Mm - K_m*(K_w*Mu**m_w)**2)**2)
    return cost

[K_w, m_w, K_f, K_m] = scipy.optimize.minimize(
    fun=cost_function,
    x0=[0, 1, 0, 0],
    args=(F_u, F_w, F_f, M_u, M_m),
    bounds=[(0, np.inf), (0, np.inf), (0, np.inf), (0, np.inf)],
).x

print(f'w(u) = {K_w:8.3f} u ^ {m_w:.3f}')
print(f'F(u) = {K_f*K_w**2:8.3f} u ^ {2*m_w:.3f}')
print(f'M(u) = {K_m*K_w**2:8.3f} u ^ {2*m_w:.3f}')
print('-------------------------------')
print(f'F(w) = {K_f:e} w ^ 2')
print(f'M(w) = {K_m:e} w ^ 2')

plt.figure()
plt.scatter(100*F_u, F_w, label='samples', color='black', s=10)
u = np.linspace(min(F_u), max(F_u), 100)
plt.plot(100*u, K_w*u**m_w, label='best fit', color='red')
plt.xlabel('throttle [%]')
plt.ylabel('rotor velocity [rad/s]')
plt.title('rotor velocity vs. throttle')
plt.grid()
plt.legend()

plt.figure()
plt.scatter(100*F_u, F_f, label='samples', color='black', s=10)
u = np.linspace(min(F_u), max(F_u), 100)
plt.plot(100*u, K_f*(K_w*u**m_w)**2, label='best fit', color='red')
plt.xlabel('throttle [%]')
plt.ylabel('thrust [N]')
plt.title('thrust vs. throttle')
plt.grid()
plt.legend()

plt.figure()
plt.scatter(100*M_u, M_m, label='samples', color='black', s=10)
u = np.linspace(min(M_u), max(M_u), 100)
plt.plot(100*u, K_m*(K_w*u**m_w)**2, label='best fit', color='red')
plt.xlabel('throttle [%]')
plt.ylabel('torque [Nm]')
plt.title('torque vs. throttle')
plt.grid()
plt.legend()

plt.figure()
plt.scatter(F_w, F_f, label='samples', color='black', s=10)
w = np.linspace(min(F_w), max(F_w), 100)
plt.plot(w, K_f*w**2, label='best fit', color='red')
plt.xlabel('rotor velocity [rad/s]')
plt.ylabel('thrust [N]')
plt.title('thrust vs. rotor velocity')
plt.grid()
plt.legend()

plt.figure()
plt.scatter(K_w*M_u**m_w, M_m, label='est. samples', color='black', s=10)
w = np.linspace(min(K_w*M_u**m_w), max(K_w*M_u**m_w), 100)
plt.plot(w, K_m*w**2, label='best fit', color='red')
plt.xlabel('rotor velocity [rad/s]')
plt.ylabel('torque [Nm]')
plt.title('torque vs. rotor velocity')
plt.grid()
plt.legend()

plt.show()
