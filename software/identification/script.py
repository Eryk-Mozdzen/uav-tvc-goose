import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import scipy.optimize
import scipy.constants

F_experiment = pd.read_csv('thrust_data.csv')
M_experiment = pd.read_csv('torque_data.csv')

F_u = F_experiment['throttle'].values
load = F_experiment['load'].values
F_v = 1000*F_u**0.7 # F_experiment['velocity'].values
F = [(l - min(load))*scipy.constants.g for l in load]

arm = 0.03
M_u = M_experiment['throttle'].values
load = M_experiment['load'].values
M_v = 1000*M_u**0.7 # M_experiment['velocity'].values
M = [(l - min(load))*scipy.constants.g*arm for l in load]

[K_w, m], _ = scipy.optimize.curve_fit(lambda u, K, m: K*u**m, np.concatenate([F_u, M_u]), np.concatenate([F_v, M_v]), bounds=(0, np.inf))
K_f = scipy.optimize.curve_fit(lambda w, K: K*w**2, F_v, F, bounds=(0, np.inf))[0][0]
K_m = scipy.optimize.curve_fit(lambda w, K: K*w**2, M_v, M, bounds=(0, np.inf))[0][0]

print(f'w(u) = {K_w:8.3f} u ^ {m:.3f}')
print(f'F(u) = {K_f*K_w**2:8.3f} u ^ {2*m:.3f}')
print(f'M(u) = {K_m*K_w**2:8.3f} u ^ {2*m:.3f}')
print('-------------------------------')
print(f'F(w) = {K_f:e} w ^ 2')
print(f'M(w) = {K_m:e} w ^ 2')

plt.figure()
plt.scatter(100*F_u, F, label='samples', color='black', s=10)

u = np.linspace(min(F_u), max(F_u), 100)
plt.plot(100*u, K_f*(K_w*u**m)**2, label='best fit', color='red')

plt.xlabel('throttle [%]')
plt.ylabel('thrust [N]')
plt.title('thrust vs. throttle')
plt.grid()
plt.legend()

plt.figure()
plt.scatter(100*M_u, M, label='samples', color='black', s=10)

u = np.linspace(min(M_u), max(M_u), 100)
plt.plot(100*u, K_m*(K_w*u**m)**2, label='best fit', color='red')

plt.xlabel('throttle [%]')
plt.ylabel('torque [Nm]')
plt.title('torque vs. throttle')
plt.grid()
plt.legend()

plt.show()
