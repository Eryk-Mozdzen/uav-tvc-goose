import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import scipy.optimize
import scipy.constants

file = pd.read_csv('thrust_data.csv')

throttle = file['throttle'].values
load = file['load'].values

thrust = [(l - load[0])*scipy.constants.g for l in load]

[K, m], _ = scipy.optimize.curve_fit(lambda x, K, m: K*x**m, throttle[1:], thrust[1:])

print(f'F(u) = {K:3.3f} u ^ {m:3.3f}')

plt.scatter(100*throttle, thrust, label='samples', color='black', s=10)

u = np.linspace(min(throttle), max(throttle), 100)
plt.plot(100*u, K*u**m, label='best fit', color='red')

plt.xlabel('throttle [%]')
plt.ylabel('thrust [N]')
plt.title('thrust vs. throttle')
plt.grid()
plt.legend()

plt.show()
