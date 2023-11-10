import numpy as np
import control
import math

K_t = 3.85756
K_w = 0.0000014188
K_tau = 0.04249
C_l = 0.43

J_xx = 0.000927790
J_yy = 0.000918280
J_zz = 0.000199379
J_r = 0.000014703

m = 0.332
l = 0.04098 + 0.01225
r = 0.0665
alpha_s = math.radians(-5)
g = 9.81

alpha_0 = -K_tau/(4*r*C_l*K_t) - alpha_s
throttle_0 = m*g/K_t
w_t0 = 1050
F_t0 = K_t*throttle_0

operating_point = np.array([
    [alpha_0],
    [alpha_0],
    [alpha_0],
    [alpha_0],
    [throttle_0]
])

A = np.array([
    [0, 0,  1,             0,             0, 0, 0],
    [0, 0,  0,             1,             0, 0, 0],
    [0, 0,  0,             J_r*w_t0/J_xx, 0, 0, 0],
    [0, 0, -J_r*w_t0/J_yy, 0,             0, 0, 0],
    [0, 0,  0,             0,             0, 0, 0],
    [0, 0,  0,             0,             0, 0, 1],
    [0, 0,  0,             0,             0, 0, 0]
])

B = np.array([
    [ 0,                0,                0,                0,               0],
    [ 0,                0,                0,                0,               0],
    [-F_t0*C_l*l/J_xx,  0,                F_t0*C_l*l/J_xx,  0,               0],
    [ 0,               -F_t0*C_l*l/J_yy,  0,                F_t0*C_l*l/J_yy, 0],
    [-F_t0*C_l*r/J_zz, -F_t0*C_l*r/J_zz, -F_t0*C_l*r/J_zz, -F_t0*C_l*r/J_zz, 0],
    [ 0,                0,                0,                0,               0],
    [ 0,                0,                0,                0,               K_t/m]
])

G = np.array([
	[1, 0, 0, 0, 0, 0, 0],
	[0, 1, 0, 0, 0, 0, 0],
	[0, 0, 0, 0, 1, 0, 0],
	[0, 0, 0, 0, 0, 1, 0]
])

Q = np.diag([
    100,
    100,
    10,
    10,
    10,
    10,
    1,

	50,
	50,
	20,
	10
])

R = np.diag([
    600,
    600,
    600,
    600,
    1000
])

N = np.zeros((11, 5))

K, _, _ = control.lqr(A, B, Q, R, N, integral_action=G)

def write_int(file, num, name):
	file.write('''constexpr int {0} = {1};\n'''.format(name, num))

def write_matrix(file, matrix, name, format='{: .4f}'):
	file.write('''constexpr Matrix<{0}, {1}> {2} = {{\n'''.format(matrix.shape[0], matrix.shape[1], name))
	for row in matrix:
		row = [format.format(val) for val in row]
		file.write('    ' + ', '.join(map(str, row)) + ',\n')
	file.write('};\n\n')

with open('src/control/controller_params.h', 'w') as file:
	file.write('// auto-generated file\n\n')
	file.write('#pragma once\n\n')
	file.write('#include <matrix.h>\n\n')
	file.write('namespace controller {\n\n')

	write_int(file, B.shape[1], 'dimU')
	write_int(file, A.shape[0], 'dimX')
	write_int(file, G.shape[0], 'dimG')
	file.write('\n')
	write_matrix(file, operating_point, 'operating_point')
	write_matrix(file, K, 'K')
	write_matrix(file, G, 'G', format='{:.0f}')

	file.write('}\n')
