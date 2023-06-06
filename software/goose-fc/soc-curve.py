#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt

def derivative(coeff):
    deriv = []
    for i in range(len(coeff)-1):
        n = len(coeff) - i - 1
        deriv.append(coeff[i]*n)
    return deriv

dataset = [
    (0.00, 2.6936),
    (0.05, 3.2567),
    (0.10, 3.4552),
    (0.15, 3.5220),
    (0.20, 3.5408),
    (0.25, 3.5750),
    (0.30, 3.6144),
    (0.35, 3.6504),
    (0.40, 3.6776),
    (0.45, 3.6982),
    (0.50, 3.7184),
    (0.55, 3.7414),
    (0.60, 3.7670),
    (0.65, 3.7954),
    (0.70, 3.8270),
    (0.75, 3.8627),
    (0.80, 3.9073),
    (0.85, 3.9628),
    (0.90, 4.0056),
    (0.95, 4.0510),
    (1.00, 4.1000)
]

soc, voc = [*zip(*dataset)]

coeff = np.polyfit(soc, voc, 11)
deriv = derivative(coeff)

plt.grid()
plt.title('Voc(SOC)')
plt.xlabel('SOC [~]')
plt.ylabel('V_oc [V]')

ls = np.linspace(0, 1, 100)
plt.plot(ls, np.poly1d(coeff)(ls), color='red', linestyle='dashed', zorder=1)
plt.scatter(soc, voc, color='black', zorder=2)

density = [c/(voc[-1]-voc[0]) for c in deriv]

print(['{:+6.2f}'.format(c) for c in coeff])
print(['{:+6.2f}'.format(c) for c in deriv])

x = [0]
while True:
    x.append(x[-1] + 0.05/np.poly1d(density)(x[-1]))
    if x[-1]>1:
        break
x = [p/x[-1] for p in x]
y = [np.poly1d(coeff)(p) for p in x]

plt.figure()
plt.grid()
plt.title('SOC(Voc)')
plt.xlabel('V_oc [V]')
plt.ylabel('SOC [~]')

coeff = np.polyfit(y, x, 11)
deriv = derivative(coeff)

ls = np.linspace(2.6936, 4.100, 100)
plt.plot(ls, np.poly1d(coeff)(ls), color='red', linestyle='dashed', zorder=1)
plt.plot(ls, np.poly1d(deriv)(ls), color='green', linestyle='dashed', zorder=1)
plt.scatter(y, x, color='black', zorder=2)

print(['{:+6.2f}'.format(c) for c in coeff])
print(['{:+6.2f}'.format(c) for c in deriv])

plt.show()
