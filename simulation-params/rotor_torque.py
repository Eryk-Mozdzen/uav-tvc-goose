import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
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

g = 9.81
arm = 0.2

samples = read_csv('rotor_torque_data.csv', ['Throttle', 'Load'])

throttle = [m[0]/100 for m in samples]
torque = [m[1]/1000*g*arm for m in samples]

K = curve_fit(lambda x, a: a*x, throttle, torque)[0][0]

print(f'M(u) = {K:3.5f} u')

plt.scatter(throttle, torque, color="black")

X = np.linspace(min(throttle), max(throttle), 100)
plt.plot(X, K*X, color="red", label="best fit")

plt.xlabel("throttle [~]")
plt.ylabel("torque [Nm]")
plt.title("throttle vs. torque")
plt.grid(True)
plt.legend()

plt.show()
