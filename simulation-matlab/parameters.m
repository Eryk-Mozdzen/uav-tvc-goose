clear
clc

%% phisical parameters

fins_max_pos = deg2rad(45);
fins_max_vel = deg2rad(180);
throttle_ramp = 0.333;

K_t = 3.85756*1.2;
K_w = 0.0000014188;
K_tau = 0.04249*1.2;

A_fin = 0.004203;
A_rot = pi*0.1^2;
C_La = 2*pi;
% C_l = 0.5*C_La*A_fin/A_rot;
C_l = 0.25*1.75;

J_xx = 0.000834286;
J_yy = 0.000826789;
J_zz = 0.000192647;
J_r = 0.000014703;

m = 0.308;
l = 0.04364 - 0.01325;
r = 0.065;
alpha_s = -deg2rad(9);

g = 9.81;

%% measurements characteristics

measurement_freq = 100;
measurement_dt = 1/measurement_freq;
variance_acc = 0.01;
variance_gyr = 0.01;
variance_mag = 0.01;
variance_dst = 0.01;

%% operating point

alpha_0 = -K_tau/(4*r*C_l*K_t) - alpha_s;
throttle_0 = m*g/K_t;

operating_point = [
    alpha_0;
    alpha_0;
    alpha_0;
    alpha_0;
    throttle_0;
]

fprintf('Nominalne wychylenie przepustnicy: %4.1f %%\n', throttle_0*100)
fprintf('Nominalne wychylenie łopatek:      %4.1f deg\n', alpha_0*180/pi)

%% linear model

% w_t0 = sqrt(m*g/K_w);
w_t0 = 1050;
F_t0 = K_t*throttle_0;

A = [
    0, 0,  1,             0,             0, 0, 0,    0, 0, 0, 0;
    0, 0,  0,             1,             0, 0, 0,    0, 0, 0, 0;
    0, 0,  0,             J_r*w_t0/J_xx, 0, 0, 0,    0, 0, 0, 0;
    0, 0, -J_r*w_t0/J_yy, 0,             0, 0, 0,    0, 0, 0, 0;
    0, 0,  0,             0,             0, 0, 0,    0, 0, 0, 0;
    0, 0,  0,             0,             0, 0, 1,    0, 0, 0, 0;
    0, 0,  0,             0,             0, 0, 0,    0, 0, 0, 0;

    1, 0,  0,             0,             0, 0, 0,    0, 0, 0, 0;
    0, 1,  0,             0,             0, 0, 0,    0, 0, 0, 0;
    0, 0,  0,             0,             1, 0, 0,    0, 0, 0, 0;
    0, 0,  0,             0,             0, 1, 0,    0, 0, 0, 0;
];

B = [
     0,                0,                0,                0,               0;
     0,                0,                0,                0,               0;
    -F_t0*C_l*l/J_xx,  0,                F_t0*C_l*l/J_xx,  0,               0;
     0,               -F_t0*C_l*l/J_yy,  0,                F_t0*C_l*l/J_yy, 0;
    -F_t0*C_l*r/J_zz, -F_t0*C_l*r/J_zz, -F_t0*C_l*r/J_zz, -F_t0*C_l*r/J_zz, 0;
     0,                0,                0,                0,               0;
     0,                0,                0,                0,               K_t/m;

     0,                0,                0,                0,               0;
     0,                0,                0,                0,               0;
     0,                0,                0,                0,               0;
     0,                0,                0,                0,               0;
];

G = [
    1, 0, 0, 0, 0, 0, 0;
    0, 1, 0, 0, 0, 0, 0;
    0, 0, 0, 0, 1, 0, 0;
    0, 0, 0, 0, 0, 1, 0;
];

%% LQR regulator

Q = diag([ ...
    7.5;
    7.5;
    1;
    1;
    2;
    5;
    3;

    1;
    1
    1;
    1;
]);

R = diag([
    500;
    500;
    500;
    500;
    1000;
]);

K = lqr(A, B, Q, R)
