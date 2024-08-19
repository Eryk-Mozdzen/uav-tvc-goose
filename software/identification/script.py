import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import scipy.optimize
import scipy.constants

def objective(params, u1, F, u2, M):
    K_f, K_m, m = params

    F_pred = K_f*u1**m
    M_pred = K_m*u2**m

    error_F = np.sum((F - F_pred)**2)
    error_M = np.sum((M - M_pred)**2)

    return error_F + error_M

F_experiment = pd.read_csv('thrust_data.csv')
M_experiment = pd.read_csv('torque_data.csv')

F_u = F_experiment['throttle'].values
load = F_experiment['load'].values
F = [(l - min(load))*scipy.constants.g for l in load]

arm = 0.03
M_u = M_experiment['throttle'].values
load = M_experiment['load'].values
M = [(l - min(load))*scipy.constants.g*arm for l in load]

K_f, K_m, m = scipy.optimize.minimize(
    fun=objective,
    x0=[1, 1, 1],
    args=(F_u, F, M_u, M),
    bounds=[(0, np.inf), (0, np.inf), (0, np.inf)],
).x

K_mf = K_m/K_f

print(f'F(u) = {K_f:.4f} u ^ {m:.4f}')
print(f'M(u) = {K_m:.4f} u ^ {m:.4f}')
print(f'M(u) = {K_mf:.4f} F(u)')

plt.figure()
plt.scatter(100*F_u, F, label='samples', color='black', s=10)

u = np.linspace(min(F_u), max(F_u), 100)
plt.plot(100*u, K_f*u**m, label='best fit', color='red')

plt.xlabel('throttle [%]')
plt.ylabel('thrust [N]')
plt.title('thrust vs. throttle')
plt.grid()
plt.legend()

plt.figure()
plt.scatter(100*M_u, M, label='samples', color='black', s=10)

u = np.linspace(min(M_u), max(M_u), 100)
plt.plot(100*u, K_m*u**m, label='best fit', color='red')

plt.xlabel('throttle [%]')
plt.ylabel('torque [Nm]')
plt.title('torque vs. throttle')
plt.grid()
plt.legend()

plt.show()
