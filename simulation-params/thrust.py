import numpy as np
import matplotlib.pyplot as plt
from dataclasses import dataclass
import sys

@dataclass
class Measurement:
    throttle: float		# PWM %
    thrust: float		# grams
    velocity: float		# RPS

data = [
	Measurement(0,		0,		0),
	Measurement(5,		16,		51),
	Measurement(10,		38,		81),
	Measurement(15,		65,		104),
	Measurement(20,		90,		122),
	Measurement(25,		113,	142),
	Measurement(30,		137,	157),
	Measurement(35,		161,	171),
	Measurement(40,		177,	181),
	Measurement(45,		197,	202),
	Measurement(50,		213,	210),
	Measurement(55,		230,	217),
	Measurement(60,		250,	221),
	Measurement(65,		267,	226),
	# Measurement(70,		291,	0),
	# Measurement(75,		320,	0),
	# Measurement(80,		345,	0),
	# Measurement(85,		369,	0),
	# Measurement(90,		385,	0),
	# Measurement(95,		390,	0),
	# Measurement(100,	390,	0)
]

g = 9.81

throttle = [measurement.throttle/100 for measurement in data]
thrust = [measurement.thrust/1000*g for measurement in data]
velocity = [measurement.velocity*np.pi for measurement in data]

K = sum(thrust)/sum(throttle)

print(f'F(u) = {K:3.5f} u')

plt.scatter(throttle, thrust, color="black")

X = np.arange(min(throttle), max(throttle) + 0.1, 0.01)
plt.plot(X, K*X, color="red", label="best fit")

plt.xlabel("throttle [~]")
plt.ylabel("thrust [N]")
plt.title("throttle vs. thrust")
plt.grid(True)
plt.legend()

Kt = sum(thrust)/sum([v**2 for v in velocity])

print(f'F(w) = {Kt:3.10f} w^2')

if len(sys.argv)>1:
	mass = float(sys.argv[1])

	w0 = np.sqrt(mass*g/Kt)

	print(f'w_0 = {w0:3.10f} rad/s')

plt.figure()

plt.scatter(velocity, thrust, color="black")

X = np.arange(min(velocity), max(velocity) + 0.1, 0.01)
plt.plot(X, [Kt*x**2 for x in X], color="red", label="best fit")

plt.xlabel("velocity [rad/s]")
plt.ylabel("thrust [N]")
plt.title("velocity vs. thrust")
plt.grid(True)
plt.legend()

plt.show()
