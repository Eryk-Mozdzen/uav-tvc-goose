clear
clc

%% phisical parameters

fins_max_pos = pi/6;
fins_max_vel = pi;
throttle_max_rate = 2;

K_t = 3.85756;
K_w = 0.0000014188;
K_tau = 0.04249;

A_fin = 0.003228;
A_rot = pi*0.1^2;
C_La = 2*pi;
C_l = 0.5*C_La*A_fin/A_rot;     % <1
C_d = 0.0;                      % <1

J_xx = 0.000878648;
J_yy = 0.000868024;
J_zz = 0.000145457;
J_r = 0.000016510;

m = 0.27768;
l = 0.05637 + 0.007;
r = 0.06475;

g = 9.81;

%% measurements characteristics

measurement_freq = 100;
measurement_dt = 1/measurement_freq;
variance_acc = 0.01;
variance_gyr = 0.01;
variance_mag = 0.01;
variance_dst = 0.01;

%% operating point

alpha_0 = [
    K_tau/(4*r*C_l*K_t);
    K_tau/(4*r*C_l*K_t);
    K_tau/(4*r*C_l*K_t);
    K_tau/(4*r*C_l*K_t);
];

throttle_0 = m*g/(K_t*(1-C_d));

operating_point = [alpha_0; throttle_0];

fprintf('Nominalne wychylenie przepustnicy: %4.1f %%\n', throttle_0*100)
fprintf('Nominalne wychylenie łopatek:      %4.1f deg\n', alpha_0(1)*180/pi)

%% linear model

w_t0 = sqrt(m*g/K_w);
F_t0 = K_t*throttle_0;

A = [
    0, 0, 0,  1,               0,               0, 0, 0;
    0, 0, 0,  0,               1,               0, 0, 0;
    0, 0, 0,  0,               0,               1, 0, 0;
    0, 0, 0,  0,               (J_r/J_xx)*w_t0, 0, 0, 0;
    0, 0, 0, -(J_r/J_yy)*w_t0, 0,               0, 0, 0;
    0, 0, 0,  0,               0,               0, 0, 0;
    0, 0, 0,  0,               0,               0, 0, 1;
    0, 0, 0,  0,               0,               0, 0, 0;
];

B = [
     0,                0,                0,                0,               0;
     0,                0,                0,                0,               0;
     0,                0,                0,                0,               0;
     0,                F_t0*C_l*l/J_xx,  0,               -F_t0*C_l*l/J_xx, 0;
     F_t0*C_l*l/J_yy,  0,               -F_t0*C_l*l/J_yy,  0,               0;
    -F_t0*C_l*r/J_zz, -F_t0*C_l*r/J_zz, -F_t0*C_l*r/J_zz, -F_t0*C_l*r/J_zz, K_tau/J_zz;
     0,                0,                0,                0,               0;
     0,                0,                0,                0,               (1-C_d)*K_t/m;
];

%% LQR regulator

Q = diag([ ...
    100;
    100;
    10;
    50;
    50;
    10;
    50;
    10;
]);

R = diag([
    1000;
    1000;
    1000;
    1000;
    1000;
]);

K = lqr(A, B, Q, R)
