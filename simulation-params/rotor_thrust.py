import numpy as np
import matplotlib.pyplot as plt
import sys
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

thrust_samples = read_csv('rotor_thrust_data.csv', ['Throttle', 'Thrust'])

if len(thrust_samples)>0:
	throttle = [m[0]/100 for m in thrust_samples]
	thrust = [m[1]/1000*g for m in thrust_samples]

	K = sum(thrust)/sum(throttle)

	print(f'F(u) = {K:3.5f} u')

	plt.scatter(throttle, thrust, color="black")

	X = np.linspace(min(throttle), max(throttle), 100)
	plt.plot(X, K*X, color="red", label="best fit")

	plt.xlabel("throttle [~]")
	plt.ylabel("thrust [N]")
	plt.title("throttle vs. thrust")
	plt.grid(True)
	plt.legend()

velocity_samples = read_csv('rotor_thrust_data.csv', ['Thrust', 'Velocity'])

if len(velocity_samples)>0:
	thrust = [m[0]/1000*g for m in velocity_samples]
	velocity = [m[1] for m in velocity_samples]

	Kt = sum(thrust)/sum([v**2 for v in velocity])

	print(f'F(w) = {Kt:3.10f} w^2')

	if len(sys.argv)>1:
		mass = float(sys.argv[1])

		w0 = np.sqrt(mass*g/Kt)

		print(f'w_0 = {w0:5.3f} rad/s')

	plt.figure()

	plt.scatter(velocity, thrust, color="black")

	X = np.linspace(min(velocity), max(velocity), 100)
	plt.plot(X, [Kt*x**2 for x in X], color="red", label="best fit")

	plt.xlabel("velocity [rad/s]")
	plt.ylabel("thrust [N]")
	plt.title("velocity vs. thrust")
	plt.grid(True)
	plt.legend()

plt.show()
