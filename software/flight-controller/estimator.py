import sympy as sp
import sympy.codegen.ast
import os
import datetime
import scipy.constants

dt = sp.Symbol('T')
g = sp.Symbol('g')

qw, qx, qy, qz = sp.symbols('q_w q_x q_y q_z')
Wx, Wy, Wz = sp.symbols('omega_x omega_y omega_z')
px, py, pz = sp.symbols('p_x p_y p_z')
vx, vy, vz = sp.symbols('v_x v_y v_z')
thetad = sp.symbols('theta_d')
p0 = sp.symbols('p_0')
wx, wy, wz = sp.symbols('w_x w_y w_z')
ax, ay, az = sp.symbols('a_x a_y a_z')

q = sp.Quaternion(qw, qx, qy, qz, norm=1)
W = sp.Matrix([Wx, Wy, Wz])
p = sp.Matrix([px, py, pz])
v = sp.Matrix([vx, vy, vz])
w = sp.Matrix([wx, wy, wz])
a = sp.Matrix([ax, ay, az])

x = sp.Matrix([
    q.to_Matrix(),
    W,
    p,
    v,
    thetad,
    p0,
])

u = sp.Matrix([
    w,
    a,
])

f = sp.Matrix([
    (q + 0.5*dt*q*sp.Quaternion(0, wx, wy, wz)).to_Matrix(),
    w,
    p + dt*v - 0.5*dt**2*(q.to_rotation_matrix()*a - sp.Matrix([0, 0, -g])),
    v - dt*(q.to_rotation_matrix()*a - sp.Matrix([0, 0, -g])),
    thetad,
    p0,
])

h_mag = q.to_rotation_matrix().transpose()*sp.Matrix([0, sp.cos(thetad), sp.sin(thetad)])
h_range = sp.Matrix([pz])
h_press = sp.Matrix([p0*sp.Pow(1 - pz/44330, 5.255)])
h_gps = sp.Matrix([px, py])
yaw = sp.atan2(2 * (qw*qz + qx*qy), 1 - 2 * (qy**2 + qz**2))
h_flow = sp.Matrix([[sp.cos(yaw), sp.sin(yaw)], [-sp.sin(yaw), sp.cos(yaw)]])*sp.Matrix([vx/pz, vy/pz]) + sp.Matrix([-Wy, Wx])

here = os.path.dirname(__file__)

os.makedirs(here + '/estimator-docs', exist_ok=True)

with open(here + '/estimator-docs/main.tex', 'w') as file:
    file.write(
        '% auto-generated file\n'
        '% ' + datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S') + '\n'
        '\n'
        '\\documentclass{article}\n'
        '\\usepackage{amsmath}\n'
        '\\usepackage[paperwidth=50cm, paperheight=50cm, margin=10mm]{geometry}\n'
        '\n'
        '\\begin{document}\n'
        '\t\\[x_k = ' + sympy.latex(x) + ' = f(x_{k-1}, u_k) = ' + sympy.latex(f) + '\\]\n'
        '\t\\[\\frac{\partial}{\partial x}f(x_{k-1}, u_k) = ' + sympy.latex(f.jacobian(x)) + '\\]\n'
        '\t\\[h_{mag}(x_k) = ' + sympy.latex(h_mag) + '\\]\n'
        '\t\\[\\frac{\partial}{\partial x}h_{mag}(x_k) = ' + sympy.latex(h_mag.jacobian(x)) + '\\]\n'
        '\t\\[h_{range}(x_k) = ' + sympy.latex(h_range) + '\\]\n'
        '\t\\[\\frac{\partial}{\partial x}h_{range}(x_k) = ' + sympy.latex(h_range.jacobian(x)) + '\\]\n'
        '\t\\[h_{press}(x_k) = ' + sympy.latex(h_press) + '\\]\n'
        '\t\\[\\frac{\partial}{\partial x}h_{press}(x_k) = ' + sympy.latex(h_press.jacobian(x)) + '\\]\n'
        '\t\\[h_{gps}(x_k) = ' + sympy.latex(h_gps) + '\\]\n'
        '\t\\[\\frac{\partial}{\partial x}h_{gps}(x_k) = ' + sympy.latex(h_gps.jacobian(x)) + '\\]\n'
        '\t\\[h_{flow}(x_k) = ' + sympy.latex(h_flow) + '\\]\n'
        '\t\\[\\frac{\partial}{\partial x}h_{flow}(x_k) = ' + sympy.latex(h_flow.jacobian(x)) + '\\]\n'
        '\\end{document}\n'
    )

with open(here + '/app/estimator.h', 'w') as file:
    file.write(
        '// auto-generated file\n'
        '// ' + datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S') + '\n'
        '\n'
        '#ifndef ESTIMATOR_H\n'
        '#define ESTIMATOR_H\n'
        '\n'
        '#include "ekf.h"\n'
        '\n'
        'extern ekf_t ekf;\n'
        'extern ekf_system_model_t system_model;\n'
        'extern ekf_measurement_model_t magnetometer_model;\n'
        'extern ekf_measurement_model_t rangefinder_model;\n'
        'extern ekf_measurement_model_t barometer_model;\n'
        'extern ekf_measurement_model_t gps_model;\n'
        'extern ekf_measurement_model_t flow_model;\n'
        '\n'
        'EKF_PREDICT_DEF(' + str(x.shape[0]) + ', ' + str(u.shape[0]) + ')\n'
        'EKF_CORRECT_DEF(' + str(x.shape[0]) + ', 1)\n'
        'EKF_CORRECT_DEF(' + str(x.shape[0]) + ', 2)\n'
        'EKF_CORRECT_DEF(' + str(x.shape[0]) + ', 3)\n'
        '\n'
        '#endif\n'
    )

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

    file.write(
        '// auto-generated file\n'
        '// ' + datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S') + '\n'
        '\n'
        '#include <math.h>\n'
        '\n'
        '#include "ekf.h"\n'
        '\n'
        '#define g ' + str(scipy.constants.g) + 'f\n'
        '#define T ' + str(0.001) + 'f\n'
        '\n'
    )
    estimator([1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 102400], 1)
    system_model(f, [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1])
    measurement_model('magnetometer', h_mag, 100)
    measurement_model('rangefinder', h_range, 10)
    measurement_model('barometer', h_press, 100)
    measurement_model('gps', h_gps, 100000)
    measurement_model('flow', h_flow, 100000)
    file.write(
        'EKF_PREDICT(' + str(x.shape[0]) + ', ' + str(u.shape[0]) + ')\n'
        'EKF_CORRECT(' + str(x.shape[0]) + ', 1)\n'
        'EKF_CORRECT(' + str(x.shape[0]) + ', 2)\n'
        'EKF_CORRECT(' + str(x.shape[0]) + ', 3)\n'
    )

os.system('pdflatex -interaction=nonstopmode -output-directory=' + here + '/estimator-docs ' + here + '/estimator-docs/main.tex')
