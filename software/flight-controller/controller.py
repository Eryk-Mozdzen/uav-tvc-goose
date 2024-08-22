import sympy as sp

ur = sp.Symbol('ur', real=True, nonnegative=True)
wr = sp.Symbol('wr', real=True)
a1, a2, a3, a0 = sp.symbols('a1 a2 a3 a0', real=True)
mw, Kw, Kf, Km = sp.symbols('mw Kw Kf Km', real=True, positive=True)
g = sp.Symbol('g', real=True, positive=True)
m, l, r, Kl = sp.symbols('m, l r Kl', real=True, positive=True)
Jxx, Jyy, Jzz, Jr = sp.symbols('Jxx Jyy Jzz Jr', real=True, positive=True)
wx, wy, wz = sp.symbols('wx wy wz', real=True)
vx, vy, vz = sp.symbols('vx vy vz', real=True)

phi = sp.Symbol('phi', real=True)
theta = sp.Symbol('theta', real=True)
psi = sp.Symbol('psi', real=True)
px = sp.Symbol('x', real=True)
py = sp.Symbol('y', real=True)
pz = sp.Symbol('z', real=True)

eta = sp.Matrix([phi, theta, psi])
w = sp.Matrix([wx, wy, wz])
p = sp.Matrix([px, py, pz])
v = sp.Matrix([vx, vy, vz])

wru = Kw*ur**mw
Fw = Kf*wr**2
Mw = Km*wr**2

Fu = Fw.subs(wr, wru)
Mu = Mw.subs(wr, wru)
F1 = Kl*Fu*a1
F2 = Kl*Fu*a2
F3 = Kl*Fu*a3
Fs = Kl*Fu*a0

J = sp.diag(Jxx, Jyy, Jzz)
W = sp.Matrix([
    [1, sp.sin(phi)*sp.tan(theta), sp.cos(phi)*sp.tan(theta)],
    [0, sp.cos(phi), -sp.sin(phi)],
    [0, sp.sin(phi)/sp.cos(theta), sp.cos(phi)/sp.cos(theta)],
])
Rz = sp.Matrix([
    [sp.cos(psi), -sp.sin(psi), 0],
    [sp.sin(psi),  sp.cos(psi), 0],
    [0, 0, 1],
])
Ry = sp.Matrix([
    [ sp.cos(theta), 0, sp.sin(theta)],
    [0, 1, 0],
    [-sp.sin(theta), 0, sp.cos(theta)],
])
Rx = sp.Matrix([
    [1, 0, 0],
    [0, sp.cos(phi), -sp.sin(phi)],
    [0, sp.sin(phi),  sp.cos(phi)],
])
R = Rz*Ry*Rx

gravity = sp.Matrix([0, 0, -g])
F_thrust = sp.Matrix([0, 0, Fu])
M_areo = sp.Matrix([0, 0, -Mu])
M_gyro = sp.Matrix([
    Jr*wru*wy,
    -Jr*wru*wx,
    0,
])
F_vanes = sp.Matrix([
    -0.5*sp.sqrt(3)*F1 + 0.5*sp.sqrt(3)*F3,
    -0.5*F1 + F2 - 0.5*F3,
    0,
])
M_vanes = sp.Matrix([
    l*(-0.5*F1 + F2 - 0.5*F3),
    l*(-0.5*sp.sqrt(3)*F1 + 0.5*sp.sqrt(3)*F3),
    -r*(F1 + F2 + F3 + 3*Fs),
])

f = sp.Matrix([
    W*w,
    J.inv()*(M_areo + M_gyro + M_vanes) - J.inv()*w.cross(J*w),
    R*v,
    (1/m)*(F_thrust + F_vanes) + R.transpose()*gravity
])

f.row_del(10)
f.row_del(9)
f.row_del(7)
f.row_del(6)
f.row_del(2)

ax = sp.Symbol('a')
ur0 = sp.solve(sp.Eq(Fu, m*g), ur)
an0 = sp.solve(sp.Eq(-r*Kl*Fu*(3*ax + 3*a0) - Mu, 0), ax)

#sp.pprint(ur0)
#sp.pprint(an0)

ur0 = ur0[0]
an0 = an0[0]

operating_point = {
    phi: 0,
    theta: 0,
    psi: 0,
    wx: 0,
    wy: 0,
    wz: 0,
    px: 0,
    py: 0,
    pz: 0,
    vx: 0,
    vy: 0,
    vz: 0,

    ur: ur0,
    a1: an0,
    a2: an0,
    a3: an0,
}

u0 = sp.Matrix([
    ur0,
    an0,
    an0,
    an0,
])

A = f.jacobian([phi, theta, wx, wy, wz, pz, vz]).subs(operating_point)
B = f.jacobian([ur, a1, a2, a3]).subs(operating_point)

u0 = sp.simplify(u0)
A = sp.simplify(A)
B = sp.simplify(B)

#sp.pprint(A)
#sp.pprint(B)

import numpy as np
import scipy.constants
import control

params = {
    mw: 0.700,
    Kw: 1000,
    Kf: 8.748045e-06,
    Km: 1.249879e-07,
    Kl: 0.21015,

    Jxx: 0.000988742,
    Jyy: 0.000981663,
    Jzz: 0.000217661,
    Jr: 0.000013658,

    m: 0.332,
    l: 0.0377 + 0.01225,
    r: 0.0665,
    a0: np.radians(-10),
    g: scipy.constants.g,
}

u0 = u0.subs(params)
A = A.subs(params)
B = B.subs(params)

u0 = np.array(u0).astype(np.float64)
A = np.array(A).astype(np.float64)
B = np.array(B).astype(np.float64)

Q = np.diag([
    100,
    100,
    10,
    10,
    10,
    10,
    1,
])

R = np.diag([
    1000,
    1000,
    1000,
    1000,
])

K, _, _ = control.lqr(A, B, Q, R)

import os
import datetime

path = os.path.dirname(__file__)

with open(os.path.join(path, 'controller.h'), 'w') as file:
    file.write(
        '// auto-generated\n'
        '// ' + datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S') + '\n'
        '\n'
        '#ifndef CONTROLLER_H\n'
        '#define CONTROLLER_H\n'
        '\n'
        '#include "arm_math.h"\n'
        '\n'
        'extern const arm_matrix_instance_f32 u0;\n'
        'extern const arm_matrix_instance_f32 K;\n'
        '\n'
        '#endif\n'
    )

with open(os.path.join(path, 'controller.c'), 'w') as file:
    def write_matrix(matrix, name, format='{: .4f}f'):
        file.write('static const float ' + name + '_data[] = {\n')
        for row in matrix:
            row = [format.format(val) for val in row]
            file.write('    ' + ', '.join(map(str, row)) + ',\n')
        file.write(
            '};\n'
            '\n'
            'const arm_matrix_instance_f32 ' + name + ' = {\n'
            '    .numRows = ' + str(matrix.shape[0]) + ',\n'
            '    .numCols = ' + str(matrix.shape[1]) + ',\n'
            '    .pData = ' + name + '_data,\n'
            '};\n'
            '\n'
        )

    file.write(
        '// auto-generated\n'
        '// ' + datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S') + '\n'
        '\n'
        '#include "arm_math.h"\n'
        '\n'
    )

    write_matrix(u0, 'u0')
    write_matrix(K, 'K')
