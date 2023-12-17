import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from scipy.constants import g
import csv

def read_csv(file, columns):
	data = []

	with open(file, newline='') as csvfile:
		reader = csv.DictReader(csvfile)

		for row in reader:
			if all(row[field] for field in columns):
				values = [float(row[field]) for field in columns]
				data.append(values)

	return data

arm = 0.2

samples = read_csv('rotor_torque_data.csv', ['Throttle', 'Load'])

# for m in samples:
# 	print(f'({m[0]/100:4.2f}, {m[1]/1000*g*arm:7.5f})')

throttle = [m[0]/100 for m in samples]
torque = [m[1]/1000*g*arm for m in samples]

plt.scatter(throttle, torque, color="blue")

throttle = [m[0]/100 for m in samples if m[1]>0]
torque = [m[1]/1000*g*arm for m in samples if m[1]>0]

K, M0 = curve_fit(lambda x, a, b: a*x - b, throttle, torque)[0]

print(f'M(u) = {K:3.5f} u - {M0:3.5f}')

plt.scatter(throttle, torque, color="black")

X = np.linspace(0, 1, 10)
plt.plot(X, K*X - M0, color="red", label="best fit")

plt.xlabel("throttle [~]")
plt.ylabel("torque [Nm]")
plt.title("throttle vs. torque")
plt.grid(True)
plt.legend()

plt.show()
