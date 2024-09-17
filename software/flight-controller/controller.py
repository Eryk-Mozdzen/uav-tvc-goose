import sympy as sp

ur = sp.Symbol('ur', real=True, nonnegative=True)
wr = sp.Symbol('wr', real=True)
a1, a2, a3, a0 = sp.symbols('a1 a2 a3 a0', real=True)
Kw1, Kw2, Kf, Km = sp.symbols('Kw1 Kw2 Kf Km', real=True, positive=True)
g = sp.Symbol('g', real=True, positive=True)
m, l, r, Kl = sp.symbols('m, l r Kl', real=True, positive=True)
Jxx, Jyy, Jzz, Jr = sp.symbols('Jxx Jyy Jzz Jr', real=True, positive=True)
wx, wy, wz = sp.symbols('wx wy wz', real=True)
vx, vy, vz = sp.symbols('vx vy vz', real=True)
px, py, pz = sp.symbols('x y z', real=True)
phi, theta, psi = sp.symbols('phi theta psi', real=True)

eta = sp.Matrix([phi, theta, psi])
w = sp.Matrix([wx, wy, wz])
p = sp.Matrix([px, py, pz])
v = sp.Matrix([vx, vy, vz])

wru = Kw1*ur**Kw2
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
    l*(0.5*sp.sqrt(3)*F1 - 0.5*sp.sqrt(3)*F3),
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

A = f.jacobian([phi, theta, psi, wx, wy, wz, pz, vz]).subs(operating_point)
B = f.jacobian([ur, a1, a2, a3]).subs(operating_point)

u0 = sp.simplify(u0)
A = sp.simplify(A)
B = sp.simplify(B)

#sp.pprint(A)
#sp.pprint(B)

import numpy as np
import scipy.constants
import control

Pl1 =  986.49264*1e-6
Pl2 = 1820.18643*1e-6
r1 = 95.33768*1e-3
r2 = 74.49534*1e-3
CLa = 2*np.pi
Rr = 0.254/2
Pr = np.pi*Rr**2

Kl_val = CLa*(2*Pl1 + 2*Pl2)/(2*Pr)
r_val = r1*Pl1/(Pl1 + Pl2) + r2*Pl2/(Pl1 + Pl2)

#print(Kl_val)
#print(r_val)

params = {
    Kw1: 6.677316e+02,
    Kw2: 5.477513e-01,
    Kf:  1.458825e-05,
    Km:  2.531647e-07,
    Kl:  Kl_val,

    Jxx: 3513658.12176*1e-9,
    Jyy: 4068713.21612*1e-9,
    Jzz: 3724881.39219*1e-9,
    Jr:    38872.17503*1e-9,

    m: 0.518,
    l: 62.89435*1e-3,
    r: r_val,
    a0: np.radians(-10),
    g: scipy.constants.g,
}

u0 = np.array(u0.subs(params)).astype(np.float64)
A = np.array(A.subs(params)).astype(np.float64)
B = np.array(B.subs(params)).astype(np.float64)

#print(np.degrees(u0[1]))

Q = np.diag([
    1000,
    1000,
    100,
    100,
    100,
    100,
    10,
    1,

    1,
    1,
    1,
    1,
])

R = np.diag([
    100,
    1000,
    1000,
    1000,
])

G = np.array([
    [1, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 0, 0, 0, 0, 0, 0],
    [0, 0, 1, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 1, 0],
])

K, _, _ = control.lqr(A, B, Q, R, integral_action=G)

for row in K:
    print(" ".join(map(str, row)))

for row in u0:
    print(" ".join(map(str, row)))
