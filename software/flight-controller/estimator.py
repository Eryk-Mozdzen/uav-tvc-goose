import sympy
import sympy.codegen.ast
from sympy import Symbol, Quaternion, Matrix
import os
from datetime import datetime
import scipy.constants

dt = Symbol('T')
g = Symbol('g')

qw, qx, qy, qz = sympy.symbols('q_w q_x q_y q_z')
px, py, pz = sympy.symbols('p_x p_y p_z')
vx, vy, vz = sympy.symbols('v_x v_y v_z')
thetad = sympy.symbols('theta_d')
p0 = sympy.symbols('p_0')
wx, wy, wz = sympy.symbols('w_x w_y w_z')
ax, ay, az = sympy.symbols('a_x a_y a_z')

q = Quaternion(qw, qx, qy, qz, norm=1)
p = Matrix([[px], [py], [pz]])
v = Matrix([[vx], [vy], [vz]])
w = Matrix([[wx], [wy], [wz]])
a = Matrix([[ax], [ay], [az]])

gravity = Matrix([[0], [0], [-g]])

u = Matrix([
    w,
    a,
])

x = Matrix([
    q.to_Matrix(),
    p,
    v,
    thetad,
    p0,
])

f = Matrix([
    (q + 0.5*dt*q*Quaternion(0, wx, wy, wz)).to_Matrix(),
    p + dt*v + 0.5*dt**2*a,
    v + dt*a,
    thetad,
    p0,
])

h_mag = q.to_rotation_matrix().transpose()*Matrix([[0], [sympy.cos(thetad)], [sympy.sin(thetad)]])
h_grav = q.to_rotation_matrix().transpose()*Matrix([[0], [0], [-1]])
h_range = Matrix([pz])
h_press = Matrix([p0*sympy.Pow(1 - pz/44330, 5.255)])
h_gps = Matrix([[px], [py]])
h_flow = Matrix([[vx], [vy]])

here = os.path.dirname(__file__)

os.makedirs(here + '/estimator-docs', exist_ok=True)

with open(here + '/estimator-docs/main.tex', 'w') as file:
    file.write('% auto-generated file\n')
    file.write('% ' + datetime.now().strftime('%Y-%m-%d %H:%M:%S') + '\n')
    file.write('\n')
    file.write('\\documentclass{article}\n')
    file.write('\\usepackage{amsmath}\n')
    file.write('\\usepackage[paperwidth=50cm, paperheight=50cm, margin=10mm]{geometry}\n')
    file.write('\n')
    file.write('\\begin{document}\n')
    file.write('\t\\[x_k = ' + sympy.latex(x) + ' = f(x_{k-1}, u_k) = ' + sympy.latex(f) + '\\]\n')
    file.write('\t\\[\\frac{\partial}{\partial x}f(x_{k-1}, u_k) = ' + sympy.latex(f.jacobian(x)) + '\\]\n')
    file.write('\t\\[h_{mag}(x_k) = ' + sympy.latex(h_mag) + '\\]\n')
    file.write('\t\\[\\frac{\partial}{\partial x}h_{mag}(x_k) = ' + sympy.latex(h_mag.jacobian(x)) + '\\]\n')
    file.write('\t\\[h_{grav}(x_k) = ' + sympy.latex(h_grav) + '\\]\n')
    file.write('\t\\[\\frac{\partial}{\partial x}h_{grav}(x_k) = ' + sympy.latex(h_grav.jacobian(x)) + '\\]\n')
    file.write('\t\\[h_{range}(x_k) = ' + sympy.latex(h_range) + '\\]\n')
    file.write('\t\\[\\frac{\partial}{\partial x}h_{range}(x_k) = ' + sympy.latex(h_range.jacobian(x)) + '\\]\n')
    file.write('\t\\[h_{press}(x_k) = ' + sympy.latex(h_press) + '\\]\n')
    file.write('\t\\[\\frac{\partial}{\partial x}h_{press}(x_k) = ' + sympy.latex(h_press.jacobian(x)) + '\\]\n')
    file.write('\t\\[h_{gps}(x_k) = ' + sympy.latex(h_gps) + '\\]\n')
    file.write('\t\\[\\frac{\partial}{\partial x}h_{gps}(x_k) = ' + sympy.latex(h_gps.jacobian(x)) + '\\]\n')
    file.write('\t\\[h_{flow}(x_k) = ' + sympy.latex(h_flow) + '\\]\n')
    file.write('\t\\[\\frac{\partial}{\partial x}h_{flow}(x_k) = ' + sympy.latex(h_flow.jacobian(x)) + '\\]\n')
    file.write('\\end{document}\n')

with open(here + '/app/estimator.h', 'w') as file:
    file.write('// auto-generated file\n')
    file.write('// ' + datetime.now().strftime('%Y-%m-%d %H:%M:%S') + '\n')
    file.write('\n')
    file.write('#ifndef ESTIMATOR_H\n')
    file.write('#define ESTIMATOR_H\n')
    file.write('\n')
    file.write('#include "ekf.h"\n')
    file.write('\n')
    file.write('extern ekf_t ekf;\n')
    file.write('extern ekf_system_model_t system_model;\n')
    file.write('extern ekf_measurement_model_t magnetometer_model;\n')
    file.write('extern ekf_measurement_model_t gravity_model;\n')
    file.write('extern ekf_measurement_model_t rangefinder_model;\n')
    file.write('extern ekf_measurement_model_t barometer_model;\n')
    file.write('extern ekf_measurement_model_t gps_model;\n')
    file.write('extern ekf_measurement_model_t flow_model;\n')
    file.write('\n')
    file.write('EKF_PREDICT_DEF(' + str(x.shape[0]) + ', ' + str(u.shape[0]) + ')\n')
    file.write('EKF_CORRECT_DEF(' + str(x.shape[0]) + ', 1)\n')
    file.write('EKF_CORRECT_DEF(' + str(x.shape[0]) + ', 2)\n')
    file.write('EKF_CORRECT_DEF(' + str(x.shape[0]) + ', 3)\n')
    file.write('\n')
    file.write('#endif\n')

with open(here + '/app/estimator.c', 'w') as file:
    functions = {
        'Pow': [
            (lambda base, exponent: exponent==2, lambda base, exponent: '(%s)*(%s)' % (base, base)),
            (lambda base, exponent: exponent!=2, lambda base, exponent: 'powf(%s, %s)' % (base, exponent))
        ],
    }

    aliases = {
        sympy.codegen.ast.real: sympy.codegen.ast.float32,
    }

    def estimator(initial, variance):
        assert len(initial) == x.shape[0]

        x_dim = x.shape[0]

        file.write('static float x_data[' + str(x_dim) + '] = {\n')
        file.write('\t')
        for i in range(x_dim):
            file.write(str(initial[i]) + ',')
            if i!=(x_dim-1):
                file.write(' ')
        file.write('\n')
        file.write('};\n')
        file.write('\n')
        file.write('static float P_data[' + str(x_dim) + '*' + str(x_dim) + '] = {\n')
        for i in range(x_dim):
            file.write('\t')
            for j in range(x_dim):
                if i==j:
                    file.write(str(variance) + ',')
                else:
                    file.write('0,')
                if j!=(x_dim-1):
                    file.write(' ')
            file.write('\n')
        file.write('};\n')
        file.write('\n')
        file.write('ekf_t ekf = {\n')
        file.write('\t.x.numRows = ' + str(x_dim) + ',\n')
        file.write('\t.x.numCols = 1,\n')
        file.write('\t.x.pData = x_data,\n')
        file.write('\t.P.numRows = ' + str(x_dim) + ',\n')
        file.write('\t.P.numCols = ' + str(x_dim) + ',\n')
        file.write('\t.P.pData = P_data,\n')
        file.write('};\n')
        file.write('\n')

    def system_model(model, variance):
        assert len(variance) == x.shape[0]

        u_dim = u.shape[0]
        x_dim = x.shape[0]
        f_used = list(model.free_symbols)
        df_used = list(model.jacobian(x).free_symbols)

        file.write('static void system_f(const float *x, const float *u, float *x_next) {\n')
        for i in range(u_dim):
            if u[i] in f_used:
                file.write('\tconst float ' + sympy.ccode(u[i]) + ' = u[' + str(i) + '];\n')
        if len(f_used)>0:
            file.write('\n')
        for i in range(x_dim):
            if x[i] in f_used:
                file.write('\tconst float ' + sympy.ccode(x[i]) + ' = x[' + str(i) + '];\n')
        if len(f_used)>0:
            file.write('\n')
        for i in range(x_dim):
            file.write('\tx_next[' + str(i) + '] = ' + sympy.ccode(model[i], user_functions=functions, type_aliases=aliases) + ';\n')
        file.write('}\n')
        file.write('\n')
        file.write('static void system_df(const float *x, const float *u, float *x_next) {\n')
        for i in range(u_dim):
            if u[i] in df_used:
                file.write('\tconst float ' + sympy.ccode(u[i]) + ' = u[' + str(i) + '];\n')
        if len(f_used)>0:
            file.write('\n')
        for i in range(x_dim):
            if x[i] in df_used:
                file.write('\tconst float ' + sympy.ccode(x[i]) + ' = x[' + str(i) + '];\n')
        if len(df_used)>0:
            file.write('\n')
        for i in range(x_dim):
            for j in range(x_dim):
                file.write('\tx_next[' + str(i*x_dim + j) + '] = ' + sympy.ccode(model.jacobian(x)[i, j], user_functions=functions, type_aliases=aliases) + ';\n')
            if i!=(x_dim-1):
                file.write('\n')
        file.write('}\n')
        file.write('\n')
        file.write('static float system_Q_data[' + str(x_dim) + '*' + str(x_dim) + '] = {\n')
        for i in range(x_dim):
            file.write('\t')
            for j in range(x_dim):
                if i==j:
                    file.write(str(variance[i]) + ',')
                else:
                    file.write('0,')
                if j!=(x_dim-1):
                    file.write(' ')
            file.write('\n')
        file.write('};\n')
        file.write('\n')
        file.write('ekf_system_model_t system_model = {\n')
        file.write('\t.Q.numRows = ' + str(x_dim) + ',\n')
        file.write('\t.Q.numCols = ' + str(x_dim) + ',\n')
        file.write('\t.Q.pData = system_Q_data,\n')
        file.write('\t.f = system_f,\n')
        file.write('\t.df = system_df,\n')
        file.write('};\n')
        file.write('\n')

    def measurement_model(name, model, variance):
        x_dim = x.shape[0]
        z_dim = model.shape[0]
        h_used = list(model.free_symbols)
        dh_used = list(model.jacobian(x).free_symbols)

        file.write('static void ' + name + '_h(const float *x, float *z) {\n')
        for i in range(x_dim):
            if x[i] in h_used:
                file.write('\tconst float ' + sympy.ccode(x[i]) + ' = x[' + str(i) + '];\n')
        if len(h_used)>0:
            file.write('\n')
        for i in range(z_dim):
            file.write('\tz[' + str(i) + '] = ' + sympy.ccode(model[i], user_functions=functions, type_aliases=aliases) + ';\n')
        file.write('}\n')
        file.write('\n')
        file.write('static void ' + name + '_dh(const float *x, float *z) {\n')
        for i in range(x_dim):
            if x[i] in dh_used:
                file.write('\tconst float ' + sympy.ccode(x[i]) + ' = x[' + str(i) + '];\n')
        if len(dh_used)>0:
            file.write('\n')
        for i in range(z_dim):
            for j in range(x_dim):
                file.write('\tz[' + str(i*x_dim + j) + '] = ' + sympy.ccode(model.jacobian(x)[i, j], user_functions=functions, type_aliases=aliases) + ';\n')
            if i!=(z_dim-1):
                file.write('\n')
        file.write('}\n')
        file.write('\n')
        file.write('static float ' + name + '_R_data[' + str(z_dim) + '*' + str(z_dim) + '] = {\n')
        for i in range(z_dim):
            file.write('\t')
            for j in range(z_dim):
                if i==j:
                    file.write(str(variance) + ',')
                else:
                    file.write('0,')
                if j!=(z_dim-1):
                    file.write(' ')
            file.write('\n')
        file.write('};\n')
        file.write('\n')
        file.write('ekf_measurement_model_t ' + name + '_model = {\n')
        file.write('\t.R.numRows = ' + str(z_dim) + ',\n')
        file.write('\t.R.numCols = ' + str(z_dim) + ',\n')
        file.write('\t.R.pData = ' + name + '_R_data,\n')
        file.write('\t.h = ' + name + '_h,\n')
        file.write('\t.dh = ' + name + '_dh,\n')
        file.write('};\n')
        file.write('\n')

    file.write('// auto-generated file\n')
    file.write('// ' + datetime.now().strftime('%Y-%m-%d %H:%M:%S') + '\n')
    file.write('\n')
    file.write('#include <math.h>\n')
    file.write('\n')
    file.write('#include "ekf.h"\n')
    file.write('\n')
    file.write('#define g ' + str(scipy.constants.g) + 'f\n')
    file.write('#define T ' + str(0.001) + 'f\n')
    file.write('\n')
    estimator([1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100000], 1)
    system_model(f, [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1])
    measurement_model('magnetometer', h_mag, 100)
    measurement_model('gravity', h_grav, 10000)
    measurement_model('rangefinder', h_range, 1000)
    measurement_model('barometer', h_press, 10000)
    measurement_model('gps', h_gps, 1000000)
    measurement_model('flow', h_flow, 1000)
    file.write('EKF_PREDICT(' + str(x.shape[0]) + ', ' + str(u.shape[0]) + ')\n')
    file.write('EKF_CORRECT(' + str(x.shape[0]) + ', 1)\n')
    file.write('EKF_CORRECT(' + str(x.shape[0]) + ', 2)\n')
    file.write('EKF_CORRECT(' + str(x.shape[0]) + ', 3)\n')

os.system('pdflatex -interaction=nonstopmode -output-directory=' + here + '/estimator-docs ' + here + '/estimator-docs/main.tex')
