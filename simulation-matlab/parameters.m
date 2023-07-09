clear
clc

%% phisical parameters

fins_max_pos = pi/6;
fins_max_vel = pi;
throttle_max_rate = 0.3;

K_w = convangvel(10000,'rpm','rad/s');
K_t = 0.0001;
C_l = 0.2;      % <1
C_d = 0.1;      % <1
C_tau = 0.02;

J_xx = 0.1;
J_yy = 0.1;
J_zz = 0.1;
J_r = 0.001;

m = 0.5;
l = 0.1;
r = 0.1;

g = 9.81;

%% measurements characteristics

measurement_freq = 100;
measurement_dt = 1/measurement_freq;
variance_acc = 0.001;
variance_gyr = 0.001;
variance_mag = 0.001;
variance_dst = 0.001;

%% operating point

alpha_0 = [
    asin(C_tau/(4*C_l*r));
    asin(C_tau/(4*C_l*r));
    asin(C_tau/(4*C_l*r));
    asin(C_tau/(4*C_l*r));
];

throttle_0 = sqrt(m*g/(K_t*(1-C_d)))/K_w;

operating_point = [alpha_0; throttle_0];

fprintf('Nominalne wychylenie przepustnicy: %4.1f %%\n', throttle_0*100)
fprintf('Nominalne wychylenie łopatek:      %4.1f deg\n', alpha_0(1)*180/pi)

%% linear model

w_t0 = K_w*throttle_0;
F_t0 = K_t*w_t0^2;

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
    -F_t0*C_l*r/J_zz, -F_t0*C_l*r/J_zz, -F_t0*C_l*r/J_zz, -F_t0*C_l*r/J_zz, 2*C_tau*throttle_0*K_t*(K_w^2)/J_zz;
     0,                0,                0,                0,               0;
     0,                0,                0,                0,               2*(1-C_d)*throttle_0*K_t*(K_w^2)/m;
];

%% LQR regulator

Q = diag([ ...
    100;
    100;
    10;

    1;
    1;
    10;

    1;
    1;
]);

R = diag([
    10;
    10;
    10;
    10;
    10000;
]);

K = lqr(A, B, Q, R)
