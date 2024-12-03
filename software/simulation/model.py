import sympy as sp
import sympy.physics.mechanics as spm

g = sp.Symbol('g')                  # gravity acceleration            [m/s^2]
m = sp.Symbol('m')                  # drone mass                      [kg]
Jr = sp.Symbol('J_r')               # rotor inertia                   [kg*m^2]
Jxx = sp.Symbol('J_xx')             # drone inertia XX                [kg*m^2]
Jyy = sp.Symbol('J_yy')             # drone inertia YY                [kg*m^2]
Jzz = sp.Symbol('J_zz')             # drone inertia ZZ                [kg*m^2]
l = sp.Symbol('l')                  # distance vane - COM X/Y axis    [m]
r = sp.Symbol('r')                  # distance vane - COM Z axis      [m]
a0 = sp.Symbol('a_0')               # passive vane angle of attack    [rad]
Kf = sp.Symbol('K_f')               # rotor force coefficient         [N*s^2/rad^2]
Km = sp.Symbol('K_m')               # rotor torque coefficient        [N*m*s^2/rad^2]
Kl = sp.Symbol('K_l')               # vane lift coefficient           [1/rad]
T1 = sp.Symbol('T_1')               # rotor time constant 1           [1/s]
T2 = sp.Symbol('T_2')               # rotor time constant 2           [1/s]

x1 = spm.dynamicsymbols('x1')       # drone COM X                     [m]
x2 = spm.dynamicsymbols('x2')       # drone COM Y                     [m]
x3 = spm.dynamicsymbols('x3')       # drone COM Z                     [m]
x4 = spm.dynamicsymbols('x4')       # drone roll                      [rad]
x5 = spm.dynamicsymbols('x5')       # drone pitch                     [rad]
x6 = spm.dynamicsymbols('x6')       # drone yaw                       [rad]
x7 = spm.dynamicsymbols('x7')       # rotor angle                     [rad]

ur = sp.Symbol('u_r')               # rotor angular velocity target   [rad/s]
a1 = sp.Symbol('alpha_1')           # vane 1 angle of attack          [rad]
a2 = sp.Symbol('alpha_2')           # vane 2 angle of attack          [rad]
a3 = sp.Symbol('alpha_3')           # vane 3 angle of attack          [rad]

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
    (R, ((1/(T1*T2)*ur - (1/(T1*T2)*x7 - ((T1 + T2)/(T1*T2)*x7.diff('t')))))*B.z),
]

vanes = [
    (a1, sp.Rational(0, 3)*sp.pi),
    (a0, sp.Rational(1, 3)*sp.pi),
    (a2, sp.Rational(2, 3)*sp.pi),
    (a0, sp.Rational(3, 3)*sp.pi),
    (a3, sp.Rational(4, 3)*sp.pi),
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

f = sp.simplify(sp.Matrix.vstack(dq, ddq).subs([(ur, 0), (a1, 0), (a2, 0), (a3, 0)]))
G = sp.simplify(sp.Matrix.vstack(dq, ddq).jacobian([ur, a1, a2, a3]))

x = sp.Matrix.vstack(q, dq)

t = sp.Symbol('t')

y_ref = sp.Matrix([
    [sp.sin(2*sp.pi*(t + sp.pi))],
    [sp.sin(2*sp.pi*(t + sp.pi/2))],
    [sp.sin(2*sp.pi*(t + sp.pi/3))],
    [sp.sin(2*sp.pi*t)/2 + 1],
])

h = sp.Matrix([
    [x[3]],
    [x[4]],
    [x[5]],
    [Kf*x[13]**2],
])

k0 = sp.Symbol('k_0')
k1 = sp.Symbol('k_1')

v = sp.Matrix([
    [y_ref.diff('t', 2)[0] - k1*(h.diff('t')[0] - y_ref.diff('t')[0]) - k0*(h[0] - y_ref[0])],
    [y_ref.diff('t', 2)[1] - k1*(h.diff('t')[1] - y_ref.diff('t')[1]) - k0*(h[1] - y_ref[1])],
    [y_ref.diff('t', 2)[2] - k1*(h.diff('t')[2] - y_ref.diff('t')[2]) - k0*(h[2] - y_ref[2])],
    [y_ref.diff('t')[3] - k0*(h[3] - y_ref[3])],
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

A = sp.simplify(A)
b = sp.simplify(b)

u = A.inv()*(v - b)

#u = sp.simplify(u)

import os

subs = {
    (x1.diff('t'), sp.Symbol('x8')),
    (x2.diff('t'), sp.Symbol('x9')),
    (x3.diff('t'), sp.Symbol('x10')),
    (x4.diff('t'), sp.Symbol('x11')),
    (x5.diff('t'), sp.Symbol('x12')),
    (x6.diff('t'), sp.Symbol('x13')),
    (x7.diff('t'), sp.Symbol('x14')),
    (x1, sp.Symbol('x1')),
    (x2, sp.Symbol('x2')),
    (x3, sp.Symbol('x3')),
    (x4, sp.Symbol('x4')),
    (x5, sp.Symbol('x5')),
    (x6, sp.Symbol('x6')),
    (x7, sp.Symbol('x7')),
}

here = os.path.dirname(__file__)
os.makedirs(f'{here}/docs', exist_ok=True)

with open(f'{here}/docs/main.tex', 'w') as file:
    file.write(
        '\\documentclass{article}\n'
        '\\usepackage{amsmath}\n'
        '\\usepackage[paperwidth=300cm, paperheight=50cm, margin=10mm]{geometry}\n'
        '\n'
        '\\begin{document}\n'
        '    \\begin{equation}\n'
        '        \\begin{bmatrix}\n'
        '            u_r\\\\\n'
        '            \\alpha_1\\\\\n'
        '            \\alpha_2\\\\\n'
        '            \\alpha_3\\\\\n'
        '        \\end{bmatrix}\n'
        '        = ' + sp.latex(u) + '\n'
        '    \\end{equation}\n'
        '    \\begin{equation}\n'
        '        A = ' + sp.latex(A) + '\n'
        '    \\end{equation}\n'
#        '    \\begin{equation}\n'
#        '        b = ' + sp.latex(b) + '\n'
#        '    \\end{equation}\n'
        '    \\begin{equation}\n'
        '        v = ' + sp.latex(v) + '\n'
        '    \\end{equation}\n'
        '\\end{document}\n'
    )

os.system(f'pdflatex -interaction=nonstopmode -output-directory={here}/docs {here}/docs/main.tex')

import numpy as np
import scipy.integrate
import scipy.constants
import matplotlib.pyplot as plt

X = sp.symbols('X1:15')

system = f + G*u

system = system.subs({
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
    T1:  0.1,
    T2:  0.01,

    k0:  100,
    k1:  100,

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
})

system = sp.lambdify([*X, t], system)

print(system(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 590, 0))

def ode_system(t, y):
    return np.array(system(*y, t)).flatten()

solution = scipy.integrate.solve_ivp(
    fun=ode_system,
    t_span=(0, 10),
    y0=[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 590],
    method='RK45',
    rtol=1e-4,
    atol=1e-5,
)

print(solution.message)

plt.figure()
plt.plot(solution.t, solution.y[3], label='phi')
plt.plot(solution.t, solution.y[4], label='theta')
plt.plot(solution.t, solution.y[5], label='psi')
plt.plot(solution.t, 1.458825e-05*solution.y[13]**2, label='thrust')
plt.xlabel('t')
plt.ylabel('y(t)')
plt.ylim(-2, 2)
plt.legend()
plt.grid()

plt.show()
