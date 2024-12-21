import sympy as sp
import sympy.physics.mechanics as spm

g = sp.Symbol('g')                  # gravity acceleration                  [m/s^2]
m = sp.Symbol('m')                  # drone mass                            [kg]
Jr = sp.Symbol('J_r')               # rotor inertia                         [kg*m^2]
Jxx = sp.Symbol('J_xx')             # drone inertia XX                      [kg*m^2]
Jyy = sp.Symbol('J_yy')             # drone inertia YY                      [kg*m^2]
Jzz = sp.Symbol('J_zz')             # drone inertia ZZ                      [kg*m^2]
l = sp.Symbol('l')                  # distance vane - COM X/Y axis          [m]
r = sp.Symbol('r')                  # distance vane - COM Z axis            [m]
a0 = sp.Symbol('a_0')               # passive vane angle of attack          [rad]
Kf = sp.Symbol('K_f')               # rotor force coefficient               [N*s^2/rad^2]
Km = sp.Symbol('K_m')               # rotor torque coefficient              [N*m*s^2/rad^2]
Kl = sp.Symbol('K_l')               # vane lift coefficient                 [1/rad]
Tr = sp.Symbol('T_r')               # rotor time constant                   [s]

x1 = spm.dynamicsymbols('x1')       # drone COM X                           [m]
x2 = spm.dynamicsymbols('x2')       # drone COM Y                           [m]
x3 = spm.dynamicsymbols('x3')       # drone COM Z                           [m]
x4 = spm.dynamicsymbols('x4')       # drone roll                            [rad]
x5 = spm.dynamicsymbols('x5')       # drone pitch                           [rad]
x6 = spm.dynamicsymbols('x6')       # drone yaw                             [rad]
x7 = spm.dynamicsymbols('x7')       # rotor angle                           [rad]

u1 = sp.Symbol('u1')                # rotor steady-state angular velocity   [rad/s]
u2 = sp.Symbol('u2')                # vane 1 angle of attack                [rad]
u3 = sp.Symbol('u3')                # vane 2 angle of attack                [rad]
u4 = sp.Symbol('u4')                # vane 3 angle of attack                [rad]

eta = sp.Matrix([
    [x4],
    [x5],
    [x6],
])

J = sp.diag(Jxx, Jyy, Jzz)
W = sp.Matrix([
    [1, 0, -sp.sin(x5)],
    [0,  sp.cos(x4), sp.cos(x5)*sp.sin(x4)],
    [0, -sp.sin(x4), sp.cos(x5)*sp.cos(x4)],
])
w = W*eta.diff('t')

K = sp.Rational(1, 2)*m*(x1.diff('t')**2 + x2.diff('t')**2 + x3.diff('t')**2) \
    + (sp.Rational(1, 2)*w.transpose()*J*w)[0] \
    + sp.Rational(1, 2)*Jr*x7.diff('t')**2
V = m*g*x3

L = K - V

W = spm.ReferenceFrame('World')
B = spm.ReferenceFrame('Body')
R = spm.ReferenceFrame('Rotor')
C = spm.Point('Center of Mass')
C.set_vel(W, x1.diff('t')*W.x + x2.diff('t')*W.y + x3.diff('t')*W.z)
B.orient_body_fixed(W, (x6, x5, x4), 'ZYX')
R.orient_axis(B, B.z, x7)

forces = [
    (C, (Kf*x7.diff('t')**2)*B.z),
    (B, (Km*x7.diff('t')**2)*B.z),
    (B, Jr*x7.diff('t')*w[1]*B.x - Jr*x7.diff('t')*w[0]*B.y),
    (R, Jr*(u1/Tr - x7.diff('t')/Tr)*B.z),
]

vanes = [
    (u2, sp.Rational(0, 3)*sp.pi),
    (a0, sp.Rational(1, 3)*sp.pi),
    (u3, sp.Rational(2, 3)*sp.pi),
    (a0, sp.Rational(3, 3)*sp.pi),
    (u4, sp.Rational(4, 3)*sp.pi),
    (a0, sp.Rational(5, 3)*sp.pi),
]
for i, (ai, dir) in enumerate(vanes):
    Fi = Kl*(Kf*x7.diff('t')**2)*ai*(-sp.sin(dir)*B.x + sp.cos(dir)*B.y)
    Pi = spm.Point(f'P{i}')
    Pi.set_pos(C, -l*B.z + r*(sp.cos(dir)*B.x + sp.sin(dir)*B.y))
    forces.append((Pi, Fi))

LM = spm.LagrangesMethod(L, [x1, x2, x3, x4, x5, x6, x7], forcelist=forces, frame=W)
LM.form_lagranges_equations()

M = LM.mass_matrix
T = LM.forcing

q = sp.Matrix([
    [x1],
    [x2],
    [x3],
    [x4],
    [x5],
    [x6],
    [x7],
])

dq = q.diff('t')

ddq = M.LUsolve(T)

f = sp.Matrix.vstack(dq, ddq).subs([(u1, 0), (u2, 0), (u3, 0), (u4, 0)])
G = sp.Matrix.vstack(dq, ddq).jacobian([u1, u2, u3, u4])

#f = sp.simplify(f)
#G = sp.simplify(G)

dynamics = f + G*sp.Matrix([u1, u2, u3, u4])

x = sp.Matrix.vstack(q, dq)

h = sp.Matrix([
    [x[3]],
    [x[4]],
    [x[5]],
    [Kf*x[13]**2],
])

v1 = sp.Symbol('v1')
v2 = sp.Symbol('v2')
v3 = sp.Symbol('v3')
v4 = sp.Symbol('v4')

v = sp.Matrix([
    [v1],
    [v2],
    [v3],
    [v4],
])

def Lie(field1, field2, order=1):
    if order==0:
        return field2
    if order==1:
        return field2.jacobian(x)*field1
    return Lie(field1, Lie(field1, field2), order-1)

relative_degree = [2, 2, 2, 1]

A = sp.Matrix.zeros(h.shape[0], h.shape[0])
b = sp.Matrix.zeros(h.shape[0], 1)

for i in range(h.shape[0]):
    for j in range(h.shape[0]):
        A[i, j] = Lie(G[:, j], Lie(f, h[i, :], relative_degree[i]-1))
    b[i] = Lie(f, h[i, :], relative_degree[i])

#A = sp.simplify(A)
#b = sp.simplify(b)

Ainv = A.inv()
control_linearized = Ainv*(v - b)

dynamics_linearized = f + G*control_linearized

import numpy as np
import scipy.constants
import os
import pickle

parameters = {
    g:   scipy.constants.g,
    m:   0.518,
    Jr:  38872.17503*1e-9,
    Jxx: 3513658.12176*1e-9,
    Jyy: 4068713.21612*1e-9,
    Jzz: 3724881.39219*1e-9,
    l:   62.89435*1e-3,
    r:   0.08182101368679512,
    a0:  np.radians(-10),
    Kf:  1.458825e-05,
    Km:  2.531647e-07,
    Kl:  0.34802890073780146,
    Tr:  0.5,
}

X = sp.symbols('x1:15')
V = sp.symbols('v1:5')
U = sp.symbols('u1:5')

symbols = {
    x1.diff('t'): X[7],
    x2.diff('t'): X[8],
    x3.diff('t'): X[9],
    x4.diff('t'): X[10],
    x5.diff('t'): X[11],
    x6.diff('t'): X[12],
    x7.diff('t'): X[13],
    x1: X[0],
    x2: X[1],
    x3: X[2],
    x4: X[3],
    x5: X[4],
    x6: X[5],
    x7: X[6],

    v1: V[0],
    v2: V[1],
    v3: V[2],
    v4: V[3],

    u1: U[0],
    u2: U[1],
    u3: U[2],
    u4: U[3],
}

#dynamics = sp.simplify(dynamics)
dynamics_linearized = sp.simplify(dynamics_linearized)
#control_linearized = sp.simplify(control_linearized)

dynamics = dynamics.subs(symbols)
dynamics_linearized = dynamics_linearized.subs(symbols)
control_linearized = control_linearized.subs(symbols)

here = os.path.dirname(__file__)

os.makedirs(f'{here}/model', exist_ok=True)

with open(f'{here}/model/parameters.pkl', 'wb') as file:
    pickle.dump(parameters, file)

with open(f'{here}/model/dynamics.pkl', 'wb') as file:
    pickle.dump(dynamics, file)

with open(f'{here}/model/dynamics_linearized.pkl', 'wb') as file:
    pickle.dump(dynamics_linearized, file)

with open(f'{here}/model/control_linearized.pkl', 'wb') as file:
    pickle.dump(control_linearized, file)

os.makedirs(f'{here}/docs', exist_ok=True)

with open(f'{here}/docs/main.tex', 'w') as file:
    file.write(
        '\\documentclass{article}\n'
        '\\usepackage{amsmath}\n'
        '\\usepackage[paperwidth=350cm, paperheight=50cm, margin=10mm]{geometry}\n'
        '\n'
        '\\begin{document}\n'
        '    \\begin{equation}\n'
        '        A^{-1} = ' + sp.latex(Ainv) + '\n'
        '    \\end{equation}\n'
#        '    \\begin{equation}\n'
#        '        \\dot{x} = f(x, u) = ' + sp.latex(dynamics) + '\n'
#        '    \\end{equation}\n'
        '    \\begin{equation}\n'
        '        \\dot{x} = f_{linearized}(x, v) = ' + sp.latex(dynamics_linearized) + '\n'
        '    \\end{equation}\n'
#        '    \\begin{equation}\n'
#        '        u = h(x, v) = ' + sp.latex(control_linearized) + '\n'
#        '    \\end{equation}\n'
        '\\end{document}\n'
    )

os.system(f'pdflatex -interaction=nonstopmode -output-directory={here}/docs {here}/docs/main.tex')
