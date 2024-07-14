import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import scipy.optimize
import scipy.constants

arm = 0.03

file = pd.read_csv('torque_data.csv')

throttle = file['throttle'].values
load = file['load'].values

torque = [(l - min(load))*scipy.constants.g*arm for l in load]

plt.scatter(100*throttle, torque, label='samples', color='black', s=10)

[K, m], _ = scipy.optimize.curve_fit(lambda x, K, m: K*x**m, throttle, torque,  bounds=(0, np.inf))

print(f'M(u) = {K:3.3f} u ^ {m:3.3f}')

u = np.linspace(min(throttle), max(throttle), 100)
plt.plot(100*u, K*u**m, label='best fit', color='red')

plt.xlabel('throttle [%]')
plt.ylabel('torque [Nm]')
plt.title('torque vs. throttle')
plt.grid()
plt.legend()

plt.show()
